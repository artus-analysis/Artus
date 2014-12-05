# -*- coding: utf-8 -*-

"""Basic plot formatting and helper module.

This module contains all the often used plotting tools
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import ROOT
import inspect
import matplotlib.pyplot as plt
import copy

import HarryPlotter.Plotting.tools.labels as labels


# TODO: delete this obsolete function
def debug(string):
	"""Print a string and the line number + file of function call."""
	log.debug(string)


# TODO: delete this obsolete function
def fail(fail_message):
	log.critical(fail_message)
	exit(1)


def printfunctions(module_list, logLevel=logging.INFO):
	"""This function prints the list of functions present in a list of modules,
	along with their docstrings.
	"""
	for module in module_list:
		log.log(logLevel, '%s' % module.__name__)
		for elem in inspect.getmembers(module, inspect.isfunction):
			log.log(logLevel, "  %s" % elem[0])
			if (elem[1].__doc__ is not None):
				log.log(logLevel, "	 ", elem[1].__doc__)


# TODO: Make this a member function of a custom plotdict class
def copyplotdict(plotdict):
	"""Make a deep copy of plotdict. This doesnt work naturally because of
	some keys like 'rootfiles' or 'analysismodules'."""
	new_plotdict =  {}
	for key in plotdict.keys():
		if key == 'analysismodules' or key =='rootfiles':
			new_plotdict[key] = copy.copy(plotdict[key])
		else:
			new_plotdict[key] = copy.deepcopy(plotdict[key])
	return new_plotdict


def printquantities(plotdict):
	"""This functions prints a list of histograms or quantites present in one 
	folder or NTuple in the input files.
	"""
	quantities = {}

	for f, name, folder in zip(plotdict['rootfiles'], plotdict['labels'], plotdict['folder']):
		log.info("Looking into folder " + folder)
		quantities[name] = []

		# Get the ntuple
		obj = f.Get(folder)
		
		#determine type of obj
		if (type(obj) == ROOT.TDirectoryFile):
			string = "Root histograms"
			func = "GetListOfKeys"
		elif (type(obj) == ROOT.TTree) or (type(obj) == ROOT.TNtuple):
			string = "Ntuple-variables"
			func = "GetListOfBranches"
		else:
			log.warning("Cannot access folder with name '%s'!" % folder)
			log.info("Available folders are:")
			for i in f.GetListOfKeys():
				log.info("\t" + i.GetName())
			return

		# Get the list of quantities from the object
		for branch in getattr(obj, func)():
		
			quantities[name] += [branch.GetTitle()]
		quantities[name] = set(quantities[name])

	if len(quantities) == 0:
		log.warning("Could not determine any quantites in this file.")
		if len(plotdict['rootfiles']) == 0:
			log.info("Please specify a file!")
		if len(plotdict['folder']) == 0:
			log.info("Please specify a folder!")
		return

	# Print the list of quantities present in ALL objects
	common_set = quantities[quantities.keys()[0]]
	for name in quantities.keys()[1:]:
		common_set = common_set.intersection(quantities[name])
	log.info('%s in ALL files:' % string)
	for q in sorted(common_set, key=lambda v: (v.upper(), v[0].islower())):
		log.info("  %s" % q)

	# Print the list of quantities that are present only in specific files
	for name in quantities.keys():
		quantities[name] = quantities[name].difference(common_set)
		if len(quantities[name]) > 0:
			log.info("Quantities only in '%s' file:" % name)
			for q in sorted(quantities[name], key=lambda v: (v.upper(), v[0].islower())):
				log.info("  %s" % q)

# remove complete plot entries from the plot dict
def removeplots(plotdict, plotIndices,
                plotLists=["folder", "weights", "x", "files", "roothistos"]):
	checkedPlotIndices = sorted([index for index in list(set(plotIndices)) if index < len(plotdict[plotLists[0]])])
	for loopIndex, plotIndex in enumerate(checkedPlotIndices):
		actualPlotIndex = plotIndex - loopIndex
		for plotList in plotLists:
			del plotdict[plotList][actualPlotIndex]

def newplot(plotdict):
	if plotdict['ratiosubplot'] == True:
		fig = plt.figure(figsize=[7, 7])
		ax1 = fig.add_subplot(111, position = [0.13, 0.37, 0.83, 0.58])
		ax2 = fig.add_subplot(111, position = [0.13, 0.12, 0.83, 0.22], sharex=ax1)
		plotdict['ratiosubplotaxes'] = ax2
		return fig, ax1
	else:
		fig = plt.figure(figsize=[7, 7])
		ax = fig.add_subplot(111)
		return fig, ax


def setaxislimits(plotdict):
	"""Set the axes (limits etc.) according to dictionary content."""

	# x axes:
	if plotdict['xview']:
		plotdict['axes'].set_xlim(plotdict['xview'][:2])
	else:
		plotdict['axes'].set_xlim(plotdict['xlims'][:2])
	if plotdict['xlog']:
		plotdict['axes'].set_xscale('log', nonposx='mask')

	# y:
	if 'y' in plotdict and plotdict['ylims'] is not None:
		plotdict['axes'].set_ylim(plotdict['ylims'][:2])
	else:
		if plotdict['log']:
			bottom = 1
			top = 10 * max(d.ymax() for d in plotdict['mplhistos'])
		else:
			bottom = 0
			top = 1.2 * max(d.ymax() for d in plotdict['mplhistos'])
		plotdict['axes'].set_ylim(top = top, bottom = bottom)

	if plotdict['log']:
		plotdict['axes'].set_yscale('log', nonposy='mask')

	if plotdict['grid']:
		plotdict['axes'].grid(True)


def save(plotdict, figure=None):
	"""Save this figure in all listed data formats.

	The standard data formats are png and pdf.
	Available graphics formats are: pdf, png, ps, eps and svg
	"""
	if not figure:
		if plotdict['figure']:
			figure = plotdict['figure']
		else:
			log.warning("No figure object to save!")

	if not plotdict:
		log.info("Please use mpl savefig if no settings are given - Saving as plot.png")
		figure.savefig("plot.png")
		return
	if not os.path.exists(plotdict['out']):
		os.makedirs(plotdict['out'])

	# a hack: this helps not to crop labels:
	if plotdict['title'] is None:
		title = plotdict['figure'].suptitle("I", color='white')
	else:
		title = plotdict['figure'].suptitle(plotdict['title'])

	name = plotdict['out'] + '/' + plotdict['filename']
	log.info(' -> Saving as')
	for f in plotdict['formats']:
		if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
			log.info('\t%s.%s' % (name, f))
			figure.savefig(name + '.' + f, bbox_inches='tight', bbox_extra_artists=[title])
			plt.close(figure)
		else:
			log.error("Failed! Output type is unknown or not supported.")

