#!/usr/bin/env python
"""Basic plot formatting and helper module.

This module contains all the often used plotting tools

"""
import os
import ROOT
import inspect
import matplotlib.pyplot as plt

import Artus.HarryPlotter.tools.labels as labels


def debug(string):
	"""Print a string and the line number + file of function call."""
	frame, filename, line_number, function_name, lines, index =\
		inspect.getouterframes(inspect.currentframe())[1]
	print "%s  (line %s in %s)" % (string, line_number, filename)


def fail(fail_message):
	print fail_message
	exit(0)


def printfunctions(module_list):
	"""This function prints the list of functions present in a list of modules,
	along with their docstrings.
	"""
	for module in module_list:
		print '%s' % module.__name__
		for elem in inspect.getmembers(module, inspect.isfunction):
			print "  %s" % elem[0]
			if (elem[1].__doc__ is not None):
				print "	 ", elem[1].__doc__


def printquantities(plotdict):
	"""This functions prints a list of histograms or quantites present in one 
	folder or NTuple in the input files.
	"""
	quantities = {}

	for f, name, folder in zip(plotdict['rootfiles'], plotdict['labels'], plotdict['folder']):
		print "Looking into folder", folder
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
			print "Cannot access folder with name '%s'" % folder
			print "Available folders are:", 
			for i in f.GetListOfKeys():
				print i.GetName()
			return

		# Get the list of quantities from the object
		for branch in getattr(obj, func)():
		
			quantities[name] += [branch.GetTitle()]
		quantities[name] = set(quantities[name])

	if len(quantities) == 0:
		print "Could not determine any quantites in this file."
		if len(plotdict['rootfiles']) == 0:
			print "Please specify a file!"
		if len(plotdict['folder']) == 0:
			print "Please specify a folder!"
		return

	# Print the list of quantities present in ALL objects
	common_set = quantities[quantities.keys()[0]]
	for name in quantities.keys()[1:]:
		common_set = common_set.intersection(quantities[name])
	print '%s in ALL files:' % string
	for q in sorted(common_set, key=lambda v: (v.upper(), v[0].islower())):
		print "  %s" % q

	# Print the list of quantities that are present only in specific files
	for name in quantities.keys():
		quantities[name] = quantities[name].difference(common_set)
		if len(quantities[name]) > 0:
			print "Quantities only in '%s' file:" % name
			for q in sorted(quantities[name], key=lambda v: (
					v.upper(), v[0].islower())):
				print "  %s" % q


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
		plotdict['axes'].set_xscale('log')

	# y:
	if 'y' in plotdict and plotdict['ylims'] is not None:
		plotdict['axes'].set_ylim(plotdict['ylims'][:2])
	else:
		if plotdict['log']:
			bottom = 1
		else:
			bottom = 0
		plotdict['axes'].set_ylim(top= 1.2 * max(d.ymax() for d in plotdict['mplhistos']), bottom = bottom)

	if plotdict['log']:
		plotdict['axes'].set_yscale('log')

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
			print "No figure object to save!"

	if not plotdict:
		print "Please use mpl savefig if no settings are given - Saving as plot.png"
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
	print ' -> Saving as',
	first = True
	for f in plotdict['formats']:
		if f in ['pdf', 'png', 'ps', 'eps', 'svg']:
			if not first:
				print ",",
			else:
				first = False
			print '%s.%s' % (name, f)
			figure.savefig(name + '.' + f, bbox_inches='tight', bbox_extra_artists=[title])
			plt.close(figure)
		else:
			print f, "failed. Output type is unknown or not supported."
