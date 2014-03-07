# -*- coding: utf-8 -*-
""" interaction with ROOT

	file open, reading of histograms and converting to a Histo class
	as well as the resp_cuts file format specific functions.

	Here is an overview of the most interesting functions besides 'openfile':
	quantity -------------------------getplotfromnick------------------> plot
								name ----------getplot-------------------> plot
	quantity --getobjectname--> name -getobject-> rootobject -root2histo-> plot
	quantity -------getobjectfromnick-----------> rootobject

	quantity:   Short name/nick name (expanded with default values) e.g. z_pt
	name:	   name of root object in file, e.g. incut_AK5PFJetsL1L2L3
	rootobject: root object (TH1D, TGraphErrors, etc.)
	plot:	  getroot Histo class object
	(The last step needs mplconvert, which is now an extra tool)

	getobjectname expands a short name with default values to the object
			name in the root file (analysis specific)
	getobject yields a root object
"""
import ROOT
import cPickle as pickle
import copy
import math
import os
import array
import numpy as np
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.dictionaries as dictionaries


# prevents Root from reading argv
ROOT.PyConfig.IgnoreCommandLineOptions = True


def ptcuts(ptvalues):
	return ["{0}<=zpt && zpt<{1}".format(*b)
		for b in zip(ptvalues[:-1], ptvalues[1:])]


def npvcuts(npvvalues):
	return ["{0}<=npv && npv<={1}".format(*b) for b in npvvalues]


def etacuts(etavalues):
	return ["{0}<=jet1abseta && jet1abseta<{1}".format(*b)
		for b in zip(etavalues[:-1], etavalues[1:])]


def alphacuts(alphavalues):
	return ["alpha<%1.2f" % b for b in alphavalues]


def openfile(filename, verbose=False, exitonfail=True):
	"""Open a root file"""
	f = ROOT.TFile(filename)
	try:
		if not f or not f.IsOpen():
			print "Can't open file:", filename
			if exitonfail:
				exit(1)
	except:
		print "Can't open file:", filename
		if exitonfail:
			exit(1)
	if verbose:
		print " * Inputfile:", filename
	return f

try:
	openfile = profile(openfile)
except NameError:
	pass  # not running with profiler


def getplot(name, rootfile, changes=None, rebin=1):
	rootobject = getobject(name, rootfile, changes)
	return root2histo(rootobject, rootfile.GetName(), rebin)


def getplotfromnick(nickname, rootfile, changes, settings):
	objectname = getobjectname(nickname, changes)
	rootobject = getobject(objectname, rootfile, changes)
	return root2histo(rootobject, rootfile.GetName(), rebin)


def getplotfromtree(nickname, rootfile, settings, twoD=False, changes=None):
	rootobject = histofromfile(nickname, rootfile, settings, twoD=False,
		changes=None)
	histo = root2histo(rootobject, rootfile.GetName(), settings['rebin'])
	rootobject.Delete()
	return histo


#def gethistofromtree(name, tree, settings, changes=None, twoD=False):


def gettreename(settings):
#	"""naming scheme
#
#	format the treename from its parts with the format of string
#	"""
#	for p in parts:
#		if p not in settings:
#			print p, "is not in settings."
#			exit(1)
#	part = [settings[p] for p in parts]
#	name = settings['folder']
	return settings['tree']


def getselection(settings, mcWeights=False):
	# create the final selection from the settings
	selection = []
	if settings['selection']:
		selection += [settings['selection']]

	# add weights

	weights = []

	if selection:
		weights = ["(" + " && ".join(selection) + ")"]
	"""
	if (mcWeights and ('noweighting' not in settings
		or not settings['noweighting'])):
		weights += ["weight"]
	if mcWeights and settings['lumi']:  # add lumi weights always?
		weights += [str(settings['lumi'])]
	if mcWeights and settings['efficiency']:
		weights += [str(settings['efficiency'])]
	if mcWeights and settings['factor']:
		weights += [str(settings['factor'])]
	"""
	selectionstr = " * ".join(weights)


	#create a copy of quantities to iterate over (to replace the from the dict):
	for key in dictionaries.ntuple_dict.keys():
		if key in selectionstr:
			selectionstr = selectionstr.replace(key, dictconvert(key))

	return selectionstr


def getbinning(quantity, settings, axis='x'):
	#missing: guess range from first entries (instead of 0,1)
	# variants: special_binning, float, int, log,
	xmin = settings[axis][0]
	xmax = settings[axis][1]
	nbins = settings['nbins']

	if nbins < 0:
		pass  # set it automatically
	if quantity in ['npv', 'npu', 'jet1nconst']:  # integer binning
		nbins = int(xmax - xmin)

	bins = [xmin + (xmax - xmin) * float(i) / nbins for i in range(nbins + 1)]
	#TODO: better log binning also for y etc.
	#print settings['xlog']
	if settings['xlog']:
		print "LOG bins is not done -> getroot.getbinning"
		xmin = max(xmin, 1.0)
		print xmin, nbins
		#bins = [xmin * (float(xmax) / xmin )
		#** (float(i) / nbins) for i in range(nbins + 1)]

	#special binning for certain quantities:
	# No, opt is the wrong place, -> dict
	bin_dict = {
		#'zpt': settings['zbins'],
		#'jet1abseta': settings['eta'],
		#'jet1eta': [-elem for elem in settings['eta'][1:][::-1]] + settings['eta'],
		#'npv': [a - 0.5 for a, b in settings['npv']] + [settings['npv'][-1][1] - 0.5]
	}

#	if settings['special_binning'] and quantity in bin_dict:
#		bins = bin_dict[quantity]

#	print "Binning of", axis, ":", nbins, "bins from",
#	print xmin, "to", xmax, "for", quantity
	return array.array('d', bins)


def histofromntuple(quantities, name, ntuple, settings, twoD=False):
	xbins = getbinning(quantities[-1], settings)
	if twoD and len(quantities) > 1:
		ybins = getbinning(quantities[0], settings, 'y')
	copy_of_quantities = quantities
	for key in dictionaries.ntuple_dict.keys():
		for quantity, i in zip(copy_of_quantities, range(len(copy_of_quantities))):
			if key in quantity:
				quantities[i] = quantities[i].replace(key, dictconvert(key))
	#TODO: TTree UserInfo: http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=16902
	#isMC = bool(ntuple.GetLeaf("npu"))

	variables = ":".join(quantities)
	selection = getselection(settings, False)

	if settings['verbose']:
		print "Creating a %s with the following selection:\n   %s" % (
			histtype, settings['selection'])

	# determine the type of histogram to be created
	if len(quantities) == 1:
		roothisto = ROOT.TH1D(name, name, len(xbins) - 1, xbins)
	elif len(quantities) == 2 and not twoD:
		roothisto = ROOT.TProfile(name, name, len(xbins) - 1, xbins)
	elif len(quantities) == 2 and twoD:
		roothisto = ROOT.TH2D(name, name, len(xbins) - 1, xbins,
			len(ybins) - 1, ybins)
	elif len(quantities) == 3:
		roothisto = ROOT.TProfile2D(name, name, len(xbins) - 1, xbins,
			len(ybins) - 1, ybins)

	if settings['verbose']:
		print "Creating a %s with the following selection:\n   %s" % (
			roothisto.ClassName(), selection)

	# fill the histogram from the ntuple
	roothisto.Sumw2()
#	if False and isMC:
#		vv = variables.split(':')
#		vv[0] += "*0.995"
#		variables = ":".join(vv)
#		print "name =", variables
	print "Weights:", selection
	ntuple.Project(name, variables, selection)

	if roothisto.ClassName() == 'TH2D':
		print "Correlation between %s and %s in %s in the selected range:  %1.5f" % (
			quantities[1], quantities[0], roothisto.GetName(),  # .split("/")[-3],
			roothisto.GetCorrelationFactor())
	print roothisto
	return roothisto

# histofromfile(quantity, rootfile, treename, settings)
def histofromfile(quantity, rootfile, settings, changes=None, twoD=False):
	"""This function returns a root object

	If quantity is an object in the rootfile it is returned.
	If not, the histo is filled from ntuple variables via the
	histofromntuple function
	"""

	histo = objectfromfile("%s/%s" % (settings['folder'][0], quantity), rootfile, warn=False)
	if histo:
		return histo

	treename = gettreename(settings)
	ntuple = objectfromfile(treename, rootfile)

	name = quantity + "_" + rootfile.GetName()
	name = name.replace("/", "").replace(")", "").replace("(", "")
	#rootfile.Delete("%s;*" % name)
	quantities = quantity.split("_")
	return histofromntuple(quantities, name, ntuple, settings, twoD=twoD)


try:
	getobjectfromtree = profile(getobjectfromtree)
except NameError:
	pass  # not running with profiler


def dictconvert(quantity):
	if quantity in dictionaries.ntuple_dict:
		return dictionaries.ntuple_dict[quantity]
	else:
		return quantity


def getobjectfromnick(nickname, rootfile, changes, rebin=1, selection=""):
	objectname = getobjectname(nickname, changes)
	return getobject(objectname, rootfile, changes, selection=selection)


def objectfromfile(name, rootfile, exact=False, warn=True):
	"""get a root object by knowing the exact name

	'exact' could be used to enforce the loading of exactly this histogram and
	not the MC version without 'Res' but it is not at the moment (strict version)
	"""
	oj = rootfile.Get(name)
#	if not oj and "Res" in name and not exact:
#		oj = rootfile.Get(name.replace("Res", ""))
#	if warn and not oj:
#		print "Can't load object", name, "from root file", rootfile.GetName()
#		exit(0)
	return oj


def getobjectname(quantity='z_mass', change={}):
	"""Build the name of a histogram according to Excalibur conventions.

	Every histogram written by Artus has a name like
	'NoBinning_incut/<quantity>_ak5PFJetsCHSL1L2L3'
	This string is returned for each quantity and any changes to the default
	can be applied via the change dictionary.

	Examples for quantities:
		z_pt, mu_plus_eta, cut_all_npv, L1_npv

	This is very old and hopefully unused!!
	"""

	"""
	# Set default values
	keys = ['bin', 'incut', 'var', 'quantity', 'algorithm', 'correction']
	selection = {'bin': 'NoBinning', 'incut': 'incut', 'var': '',
		'quantity': '<quantity>', 'algorithm': 'AK5PFJetsCHS',
		'correction': 'L1L2L3Res'}

	hst = ''
	for k in change:
		if k not in selection:
			print k, "is no valid key. Valid keys are: ", keys
			exit(1)
	# apply requested changes
	selection.update(change)
	# make a prototype name and correct it
	for k in keys:
		hst += selection[k] + '_'
	hst = hst.replace('Jets_', 'Jets').replace('__', '_')
	hst = hst.replace('_L1', 'L1')[:-1].replace('_<quantity>', '/' + quantity)
	"""
	return "genericName"


def saveasroot(rootobjects, opt, settings):
	filename = opt.out + "/%s.root" % settings['filename']
	f = ROOT.TFile(filename, "UPDATE")
	for rgraph, name in zip(rootobjects, settings['labels']):
		plotname = settings['root']  # "_".join([settings['root'], name])
		print "Saving %s in ROOT-file %s" % (plotname, filename)
		rgraph.SetTitle(plotname)
		rgraph.SetName(plotname)
		rgraph.Write()
	f.Close()


def rootdivision(rootobjects):
	#convert TProfiles into TH1Ds because ROOT cannot correctly divide TProfiles
	if (rootobjects[0].ClassName() != 'TH1D' and
			rootobjects[1].ClassName() != 'TH1D'):
		rootobjects[0] = ROOT.TH1D(rootobjects[0].ProjectionX())
		rootobjects[1] = ROOT.TH1D(rootobjects[1].ProjectionX())
	else:
		rootobjects[1].Scale(rootobjects[0].Integral() /
			rootobjects[1].Integral())
	rootobjects[0].Divide(rootobjects[1])

	#account for error in empty bins:
	for n in range(rootobjects[0].GetNbinsX()):
		if ((rootobjects[0].GetBinError(n) < 1e-5
								and rootobjects[0].GetBinContent(n) > 0.1)
								or rootobjects[0].GetBinContent(n) > 1.15):
			rootobjects[0].SetBinError(n, 0.1)

	return(rootobjects[0])


class Fitfunction:
	"""For now this can only handle linear functions intended for extrapolation

	Probably unused
	"""
	def __init__(self, x=None, y=None, yerr=None, chi2=None, ndf=None):
		if x is None:
			x = [0.0, 0.3]
		if y is None:
			y = [1.0, 0.99]
		if yerr is None:
			yerr = [0.0, 0.0]
		if chi2 is None:
			chi2 = -1
		if ndf is None:
			ndf = -1
		self.function = '(y1-y0)/(x1-x0)*x + y0'
		self.x = x
		self.y = y
		self.yerr = yerr
		self.chi2 = chi2
		self.ndf = ndf

	def __del__(self):
		pass

	def __str__(self):
		return ("y = %s \n  Parameters: x = %s, y = %s, yerr = %s\n  " +
			"Fit: chi2 / ndf = %1.5f / %d" % (
			(self.function).replace('(y1-y0)/(x1-x0)',
			str(float(self.y[1] - self.y[0]) /
				(self.x[1] - self.x[0]))).replace('y0', str(self.y[0])),
			str(self.x),
			str(self.y),
			str(self.yerr),
			self.chi2,
			self.ndf))

	def __len__(self):
		return len(self.y)

	def f(self, x):
		return float(self.y[1] - self.y[0]) / (
			self.x[1] - self.x[0]) * x + float(self.y[0])

	def ferr(self, x):
		result = float(self.yerr[1] - self.yerr[0]) / (
			self.x[1] - self.x[0]) * x + float(self.yerr[0])
		assert result >= 0
		return result

	def k(self):
		return float(self.y[0] / self.f(0.2))

	def kerr(self):
		return (float(self.yerr[0]) / self.f(0.2) -
			self.y[0] / self.f(0.2) / self.f(0.2) * self.ferr(0.2))

	def plotx(self, left=0.0, right=0.34):
		assert left < right
		return [left + x / 100.0 for x in range((right - left) * 100)]

	def ploty(self, left=0.0, right=0.34):
		assert left < right
		return [self.f(x) for x in self.plotx(left, right)]

	def plotyh(self, left=0.0, right=0.34):
		assert left < right
		return [self.f(x) + self.ferr(x) for x in self.plotx(left, right)]

	def plotyl(self, left=0.0, right=0.34):
		assert left < right
		return [self.f(x) - self.ferr(x) for x in self.plotx(left, right)]


def dividegraphs(graph1, graph2):
	assert graph1.ClassName() == 'TGraphErrors'
	assert graph2.ClassName() == 'TGraphErrors'
	assert graph1.GetN() == graph2.GetN()
	result = ROOT.TGraphErrors(graph1.GetN())

	for i in range(graph1.GetN()):
		x1, y1, dx1, dy1 = getgraphpoint(graph1, i)
		x2, y2, dx2, dy2 = getgraphpoint(graph2, i)
		if y2 == 0 or y1 == 0:
			print "Division by zero!"
		else:
			result.SetPoint(i, 0.5 * (x1 + x2), y1 / y2)
			result.SetPointError(i, 0.5 * (abs(dx1) + abs(dx2)),
				abs(y1 / y2) * math.sqrt((dy1 / y1) ** 2 + (dy2 / y2) ** 2))
	return result


def getgraphpoint(graph, i):
		a = ROOT.Double(0.0)
		b = ROOT.Double(0.0)
		graph.GetPoint(i, a, b)
		return float(a), float(b), graph.GetErrorX(i), graph.GetErrorY(i)


def getgraph(x, y, f, opt, settings, changes=None, key='var', var=None,
		drop=True, root=True, median=False, absmean=False):
	""" get a Histo easily composed from different folders

		get a Histo with quantity 'y' plotted over quantity 'x' from file 'f'
		and the settings in 'changes'. The x axis is the variation in 'var'.
	"""

	settings = utils.apply_changes(settings, changes)
	try:
		f1 = f[0]
		f2 = f[1]
	except:
		f1 = f
		f2 = None

	settings['special_binning'] = True
	settings['rebin'] = 1

	settings['x'] = utils.getaxislabels_list(x)[:2]
	xvals = getplotfromtree('_'.join([x, x]), f, settings)
	settings['x'] = utils.getaxislabels_list(y)[:2]
	yvals = getplotfromtree('_'.join([y, x]), f, settings)

	graph = ROOT.TGraphErrors()

	for i, x, y, xerr, yerr in zip(range(len(yvals.y) - 1),
			xvals.y, yvals.y, xvals.yerr, yvals.yerr):
		graph.SetPoint(i, x, y)
		graph.SetPointError(i, xerr, yerr)

	return graph


def getgraphratio(x, y, f1, f2, opt, settings, changes={}, key='var', var=None,
		drop=True, median=False, absmean=False):
	if f2 is None:
		return getgraph(x, y, f1, opt, settings, changes, key, var, drop,
			True, median, absmean)
	graph1 = getgraph(x, y, f1, opt, settings, changes, key, var, drop,
		True, median, absmean)
	graph2 = getgraph(x, y, f2, opt, settings, changes, key, var, drop,
		True, median, absmean)
	return dividegraphs(graph1, graph2)


def histomedian(histo):

	import array
	nBins = histo.GetXaxis().GetNbins()
	x = []
	y = []
	for i in range(nBins):
		x += [ROOT.Double(histo.GetBinCenter(i))]
		y += [ROOT.Double(histo.GetBinContent(i))]
	assert len(x) == len(y)
	assert len(x) == nBins
	x = array.array('d', x)
	y = array.array('d', y)

	return ROOT.TMath.Median(nBins, x, y)


def histoabsmean(histo):
	absEta = histo.Clone()
	n = absEta.GetNbinsX()	   # 200
	assert n % 2 == 0
	for i in range(1, n / 2 + 1):  # 1..100 : 200..101
		absEta.SetBinContent(n + 1 - i, histo.GetBinContent(i) +
			histo.GetBinContent(n - i + 1))
		absEta.SetBinContent(i, 0)
