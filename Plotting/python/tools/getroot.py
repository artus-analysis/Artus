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

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import cPickle as pickle
import copy
import math
import random
import os
import array
import numpy as np

import HarryPlotter.Plotting.tools.utils as utils

# prevents Root from reading argv
ROOT.PyConfig.IgnoreCommandLineOptions = True


def openfile(filename, verbose=False, exitonfail=True):
	"""Open a root file"""
	f = ROOT.TFile(filename)
	try:
		if not f or not f.IsOpen():
			log.error("Can't open file: " + filename)
			if exitonfail:
				exit(1)
	except:
		log.error("Can't open file: " + filename)
		if exitonfail:
			exit(1)
	log.debug(" * Inputfile: " + filename)
	return f

try:
	openfile = profile(openfile)
except NameError:
	pass  # not running with profiler

"""
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
"""

def histofromntuple(rootfile, objectname, ntuple, plotdict):

	log.info("Using file %s" % rootfile.GetName())
	index = plotdict['rootfiles'].index(rootfile)

	if plotdict['xlims'] == None:
		plotdict['xlims'] = [ntuple.GetMinimum(plotdict['x'][index])*1.01, ntuple.GetMaximum(plotdict['x'][index])*1.01]
		if plotdict['xlims'] == [0.0, 0.0]:
			log.warning("Axis limits could not be determined! Fallback to [0, 1]")
			plotdict['xlims'] = [0.0, 1.0]
		else:
			log.info("Min/Max x values: " + str(plotdict['xlims'][0]) + " " + str(plotdict['xlims'][1]))

	if plotdict['xbins'] == []:
		plotdict['xbins'] = np.linspace(plotdict['xlims'][0], plotdict['xlims'][1], plotdict['nbins']+1)
		xbins = plotdict['xbins']
	else:
		xbins = np.array(plotdict['xbins'], 'd')

	if plotdict['type'] == '2D':
		if plotdict['ylims'] == None:
			plotdict['ylims'] = [ntuple.GetMinimum(plotdict['y'][index])*1.01, ntuple.GetMaximum(plotdict['y'][index])*1.01]
		if plotdict['ylims'] == [0.0, 0.0]:
			log.warning("Axis limits could not be determined! Fallback to [0, 1]")
			plotdict['ylims'] = [0.0, 1.0]
		plotdict['ybins']  = np.linspace(plotdict['ylims'][0], plotdict['ylims'][1], plotdict['nbins']+1)

	variables = plotdict['x'][index]
	if len(plotdict['y']) > 0:
		variables = "%s:%s" % (plotdict['y'][index], variables)
	if 'z' in plotdict and len(plotdict['z']) > 0:
		variables = "%s:%s" % (plotdict['z'][index], variables)

	# give each histogram a different name
	name = "_".join([objectname, str(index)])

	# determine the type of histogram to be created
	if len(variables.split(":")) == 1:
		roothisto = ROOT.TH1D(name, objectname, len(xbins) - 1, xbins)
	elif len(variables.split(":")) == 2 and not plotdict['type'] == '2D':
		roothisto = ROOT.TProfile(name, objectname, len(xbins) - 1, xbins)
	elif len(variables.split(":")) == 2 and plotdict['type'] == '2D':
		roothisto = ROOT.TH2D(name, objectname, len(xbins) - 1, xbins,
			len(plotdict['ybins']) - 1, plotdict['ybins'])
	elif len(variables.split(":")) == 3:
		roothisto = ROOT.TProfile2D(name, objectname, len(xbins) - 1, xbins,
			len(plotdict['ybins']) - 1, plotdict['ybins'])

	log.debug("Creating a %s with the following weight:\n   %s" % (roothisto.ClassName(), plotdict['weights'][index]))

	# fill the histogram from the ntuple
	roothisto.Sumw2()
	ntuple.Project(name, variables, plotdict['weights'][index])

	if roothisto.ClassName() == 'TH2D':
		log.info("Correlation between %s and %s in %s in the selected range:  %1.5f" % (
			variables.split(':')[1], variables.split(':')[0], roothisto.GetName(),
			roothisto.GetCorrelationFactor()))

	return roothisto


def histofromfile(rootfile, objectname, folder, plotdict):
	"""This function returns a root object

	If quantity is an object in the rootfile it is returned.
	If not, the histo is filled from ntuple variables via the
	histofromntuple function
	"""
	path = objectname
	if folder is not None:
		path = "%s/%s" % (folder, path)
	
	histo = rootfile.Get(path)
	if histo:
		histo.Rebin(plotdict['rebin'])
		return histo

	ntuple = rootfile.Get(folder)

	name = objectname + "_" + rootfile.GetName()
	name = name.replace("/", "").replace(")", "").replace("(", "")
	return histofromntuple(rootfile, objectname, ntuple, plotdict)


try:
	getobjectfromtree = profile(getobjectfromtree)
except NameError:
	pass  # not running with profiler


def save_roothistos(plotdict):
	"""Save the root histograms to disk."""
	filename = "%s/%s_histos.root" % (plotdict['out'], plotdict['filename'])
	f = ROOT.TFile(filename, "UPDATE")
	for rgraph, histoname in zip(plotdict['roothistos'], plotdict['labels']):
		log.info("Saving %s in ROOT-file %s" % (histoname, filename))
		rgraph.SetTitle(histoname)
		rgraph.SetName(histoname)
		rgraph.Write()
	f.Close()


def rootdivision(rootobjects):
	if len(rootobjects) < 2:
		log.error("rootdivision need at least two rootobjects!")
		return
	"""Return the quotient of two ROOT histograms."""
	#convert TProfiles into TH1Ds because ROOT cannot correctly divide TProfiles
	if (rootobjects[0].ClassName() != 'TH1D' and
			rootobjects[1].ClassName() != 'TH1D'):
		rootobjects[0] = ROOT.TH1D(rootobjects[0].ProjectionX())
		rootobjects[1] = ROOT.TH1D(rootobjects[1].ProjectionX())
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
			log.error("Division by zero!")
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
