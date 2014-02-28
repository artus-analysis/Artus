#!/usr/bin/env python
"""Basic plot formatting and helper module.

This module contains all the often used plotting tools

"""
import os
import copy
#import glob
#import inspect
#import artus

import matplotlib.pyplot as plt

import Artus.Plotting.tools.labels as labels


def debug(string):
	"""Print a string and the line number + file of function call."""
	frame, filename, line_number, function_name, lines, index =\
		inspect.getouterframes(inspect.currentframe())[1]
	print "%s  (line %s in %s)" % (string, line_number, filename)


def fail(fail_message):
	print fail_message
	exit(0)


def printfiles(filelist):
	if len(filelist) > 0:
		print "Data file:", filelist[0]
	else:
		print "No input files given!"
		exit(0)
	if len(filelist) == 2:
		print "MC file:", filelist[1]
	elif len(filelist) > 2:
		print "MC files:", ", ".join(filelist[1:])


def printfunctions(module_list):
	for module in module_list:
		print '\033[92m%s' % module.__name__
		for elem in inspect.getmembers(module, inspect.isfunction):
			if (inspect.getargspec(elem[1])[0][:2] == ['files', 'opt']):
				print "\033[93m  %s \033[0m" % elem[0]
				if (elem[1].__doc__ is not None):
					print "	 ", elem[1].__doc__


def printquantities(files, opt):
	quantities = {}
	treename = "_".join([opt.folder,  opt.correction])

	for f, name in zip(files, opt.labels):
		quantities[name] = []

		# Get the ntuple
		ntuple = f.Get(treename)
		if not ntuple and "Res" in treename:
			ntuple = f.Get(treename.replace("Res", ""))

		# Get the list of quantities from the ntuple
		for branch in ntuple.GetListOfBranches():
			quantities[name] += [branch.GetTitle()]
		quantities[name] = set(quantities[name])

	# Print the list of quantities present in ALL Ntuples
	common_set = quantities[quantities.keys()[0]]
	for name in quantities.keys()[1:]:
		common_set = common_set.intersection(quantities[name])
	print '\033[92m%s\033[0m' % "Quantities in ALL files:"
	for q in sorted(common_set, key=lambda v: (v.upper(), v[0].islower())):
		print "  %s" % q

	# Print the list of quantities that are present only in specific files
	for name in quantities.keys():
		quantities[name] = quantities[name].difference(common_set)
		if len(quantities[name]) > 0:
			print '\033[92m%s\033[0m' % "Quantities only in '%s' file:" % name
			for q in sorted(quantities[name], key=lambda v: (
					v.upper(), v[0].islower())):
				print "  %s" % q


def get_selection(settings):
	"""Prepare the selections."""

	if 'selection' in settings and settings['selection'] is not None:
		selections = settings['selection'].split(" && ")
	else:
		selections = []
	return " && ".join(list(set(selections)))


def apply_changes(settings, changes):
	""" This function updates the settings dictionary with the changes
		function in a way that the selection is not overwritten but
		combined.
	"""
	nsettings = copy.deepcopy(settings)
	if changes is not None:
		if 'selection' in changes and settings['selection'] is not None:
			selection = " && ".join([changes['selection'], settings['selection']])
			nsettings.update(changes)
			nsettings['selection'] = selection
		else:
			nsettings.update(changes)
	return nsettings


def getsettings(opt, changes=None, settings=None, quantity=None):
	"""Create the local settings dictionary.

		The customizable parameters can be accessed via the settings directory
		that is created from the global 'opt' module and optional other arguments.
	"""
	#opt = copy.deepcopy(opt)
	# if a setting dictionary is given, apply changes (if any)
	if settings is not None:
		settings = apply_changes(settings, changes)
		return settings

	# 1. create a local copy of the default_settings
	settings = copy.deepcopy(opt.default_options)

	# 2. overwrite the default_settings with
	# settings(function argument, e.g. from dictionary):
	if changes is not None:
		settings = apply_changes(settings, changes)

	# 3. overwrite the default_settings with user-settings (=command line):
	if opt.user_options is not {}:
		settings = apply_changes(settings, opt.user_options)

	# 4. create the local settings for quantites and axes:
	if quantity is not None and settings['xynames'] is None:
		settings['xynames'] = quantity.split("_")[::-1]

		if len(settings['xynames']) < 2:
			settings['xynames'] += ['events']
	if settings['x'] is None:
		settings['x'] = labels.getaxislabels_list(settings['xynames'][0])[:2]

	if settings['y'] is None:
		settings['y'] = labels.getaxislabels_list(settings['xynames'][1])[:2]

#	settings['folder'] = opt.folder
#	print settings['folder']

	return settings


def getdefaultsubtexts():
	return ["a)", "b)", "c)", "d)", "e)", "f)", "g)", "h)", "i)", "j)", "k)",
			"l)", "m)", "n)", "o)", "p)", "q)", "r)"]


def showoptions(opt):
	print "Options:"
	for o, v in opt.__dict__.items():
		print "   {0:11}: {1}".format(o, v)
	print "matplotlib settings:"
	for k, v in plotrc.getstyle(opt.layout).items():
		print "   {0:24}: {1}".format(k, v)


def getfactor(lumi, fdata, fmc, quantity='z_phi', change={}):
	"""Get the normalization factor for the f_data file w.r.t. f_mc."""
	histo_data = getroot.getplot(quantity, fdata, change)
	histo_mc = getroot.getplot(quantity, fmc, change)
	histo_mc.scale(lumi)
	print "	>>> The additional scaling factor is:", (
		histo_data.ysum() / histo_mc.ysum())
	return histo_data.ysum() / histo_mc.ysum()


def getalgorithms(algorithm):
	if "AK7" in algorithm:
		algorithms = ['AK7PFJets', 'AK7PFJetsCHS']
	else:
		algorithms = ['AK5PFJets', 'AK5PFJetsCHS']
	return algorithms


def getgenname(opt):
	gen = 'AK5GenJets'
	return gen


def nicetext(s):  # to dictionaries
	if "run" in s:
		return r"Time dependence for " + nicetext(s[:-4])
	elif s in ['z_pt', 'zpt']:
		return r"$p_\mathrm{T}^\mathrm{Z}$"
	elif s in ['jet1_pt', 'jet1pt']:
		return r"$p_\mathrm{T}^\mathrm{Jet 1}$"
	elif s in ['jet2_pt', 'jet2pt']:
		return r"$p_\mathrm{T}^\mathrm{Jet 2}$"
	elif s in ['jet1eta', 'eta']:
		return r"$\eta^\mathrm{Jet1}$"
	elif s in ['jets_valid', 'jetsvalid']:
		return r"Number of valid jets"
	elif s == 'npv':
		return 'NPV'
	elif s == 'alpha':
		return r"$\alpha$"
	elif s == 'balresp':
		return r"$p_\mathrm{T}$ balance"
	elif s == 'baltwojet':
		return r"two-jet balance"
	elif s == 'mpfresp':
		return "MPF response"
	elif s == 'sumEt':
		return r"$\sum E^\mathrm{T}$"
	elif s == 'METsumEt':
		return r"Total transverse energy $\sum E^\mathrm{T}$"
	elif s == 'METpt':
		return r"$p_\mathrm{T}^\mathrm{MET}$"
	elif s == 'METphi':
		return r"$\phi^\mathrm{MET}$"
	elif s == 'met':
		return r"E_\mathrm{T}^\mathrm{miss}"
	elif s == 'MET':
		return r"E_\mathrm{T}^\mathrm{miss}"
	elif s == 'muonsvalid':
		return "Number of valid muons"
	elif s == 'muonsinvalid':
		return "Number of invalid muons"
	elif s == 'muplus':
		return "mu plus"
	elif s == 'muminus':
		return "mu minus"
	elif s == 'leadingjet':
		return r"Leading \/Jet"
	elif s == 'secondjet':
		return r"Second \/Jet"
	elif s == 'leadingjetsecondjet':
		return r"Leading\/ Jet,\/ Second\/ Jet"
	elif s == 'jet1':
		return r"Leading Jet"
	elif s == 'jet2':
		return r"Second \/Jet"
	elif s == 'z':
		return r"Z"
	elif s == 'genz':
		return r"GenZ"
	elif s == 'leadingjetMET':
		return r"Leading\/ Jet,\/ MET"
	elif s == 'jet1MET':
		return r"Leading\/ Jet,\/ MET"
	elif s == 'zMET':
		return r"Z, MET"
	elif s == 'jet2toZpt':
		return r"2nd Jet Cut"
	return s


#def getreweighting(datahisto, mchisto, drop=True):
#	if drop:
#		datahisto.dropbin(0)
#		datahisto.dropbin(-1)
#		mchisto.dropbin(0)
#		mchisto.dropbin(-1)
#	reweighting = []
#	for i in range(len(mchisto)):
#		if i > 13:
#			break
#		reweighting.append(datahisto.y[i] / mchisto.y[i])
#	return reweighting


def newplot(ratio=False, run=False, subplots=1, subplots_X=None,
															subplots_Y=None):
	fig = plt.figure(figsize=[7, 7])
	#fig.suptitle(opt.title, size='xx-large')
	# Get 4 config numbers: FigXsize, FigYsize, NaxesY, NaxesX
	if subplots is not 1:
		d = {3: 3, 2: 2, 7: 2}
		if subplots in d:
			x = d[subplots]
			y = int(round(subplots / float(x)))
		elif subplots_Y is not None:
			y = subplots_Y
			x = int(round(subplots / float(y)))
		elif subplots_X is not None:
			x = subplots_X
			y = int(round(subplots / float(x)))
		else:
			y = int(math.sqrt(subplots))
			x = int(round(subplots / float(y)))
		if x * y < subplots:
			x = x + 1
		if run:
			a = [14 * x, 7 * y, y, x]
		else:
			a = [7 * x, 7 * y, y, x]
		fig = plt.figure(figsize=[a[0], a[1]])  # apply config numbers
		ax = [fig.add_subplot(a[2], a[3], n + 1) for n in range(subplots)]
		return fig, ax
	elif ratio:
		ax = fig.add_subplot(111, position=[0.13, 0.35, 0.83, 0.58])
		ax.number = 1
		ax.set_xlabel([])
		ratio = fig.add_subplot(111, position=[0.13, 0.12, 0.83, 0.22], sharex=ax)
		ratio.number = 2
		ratio.axhline(1.0, color='gray', lw=1)
		return fig, ax, ratio
	elif run:
		fig = plt.figure(figsize=[14, 7])
		ax = fig.add_subplot(111)
		ax.number = 1
		return fig, ax
	else:
		ax = fig.add_subplot(111)
		ax.number = 1
		return fig, ax
	return fig


def setaxislimits(ax, settings):
	"""
	Set the axis limits from changes and or options. Default operation mode is:
		1. By default, axis limits are taken from the dictionary
		2. If limits are given in changes, override the dictionary values
		3. If limits are given in opt (command line values), override the values
			from dictionary or changes
	"""
	if len(settings['x']) >= 2:
		ax.set_xlim(settings['x'][0], settings['x'][1])

	if hasattr(ax, 'number') and (len(settings['y']) >= 2 * ax.number):
		ax.set_ylim(settings['y'][2 * ax.number - 2:][0],
					settings['y'][2 * ax.number - 2:][1])
	elif not hasattr(ax, 'number'):
		ax.set_ylim(settings['y'][0], settings['y'][1])


def getdefaultfilename(quantity, opt, settings):
	"""This function creates a default filename based on quantity, changes and
		algorithm/correction.
	"""
	if 'filename' in settings and settings['filename'] is not None:
		return settings['filename']
	else:
		filename = quantity

	#remove special characters:
	for char in ["*", "/", " "]:
		filename = filename.replace(char, "_")
	return filename


def Save(figure, settings=None, crop=True, pad=None):
	"""Save this figure in all listed data formats.

	The standard data formats are png and pdf.
	Available graphics formats are: pdf, png, ps, eps and svg
	"""
	if not settings:
		print "Please use mpl savefig if no settings are given"
		figure.savefig("plot.png")
		return
	print settings, type(settings)
	if not os.path.exists(settings['out']):
		os.makedirs(settings['out'])

	name = settings['out'] + '/' + settings['filename']
	name = name.replace("PFJets", "PF")
	print ' -> Saving as',
	if settings is not None and settings['title'] is not "":
		title = figure.suptitle(settings['title'], size='large')
	# this helps not to crop labels, but it is ugly, please change
	elif crop:
		title = figure.suptitle("I", color='white')
	first = True
	for f in settings['formats']:
		if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
			if not first:
				print ",",
			else:
				first = False
			print name + '.' + f
			if crop:
				if pad is not None:
					figure.savefig(name + '.' + f, bbox_inches='tight',
						bbox_extra_artists=[title], pad_inches=pad)
				else:
					figure.savefig(name + '.' + f, bbox_inches='tight',
						bbox_extra_artists=[title])
			else:
				figure.savefig(name + '.' + f)
			plt.close(figure)

		else:
			print f, "failed. Output type is unknown or not supported."
