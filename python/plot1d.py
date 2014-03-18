#### todo: adjust to dictionary plotdict to save root files
#### todo: allow interaction with external modules
#### todo: save root histograms for import/export
#### todo: save command-line settings to reproduce the plot later/ plotfromdict
#### todo: enable ratiosubplot


# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

	This module is dedicated to the creating 1D plots.
"""
import numpy
import math
import ROOT
import sys
import os
import copy

import Artus.HarryPlotter.tools.getroot as getroot
import Artus.HarryPlotter.tools.mplconvert as mplconvert
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.tools.fit as fit


def plot1d(plotdict):
	"""Function for all data/MC comparison plots for basic quantities."""

	print "A %s plot is created" % (plotdict["plot"])

	get_root_histos(plotdict)

	if plotdict['backend'] == 'root':
		plot1d_root(plotdict)
	elif plotdict['backend'] == 'mpl':
		plot1d_mpl(plotdict)
		utils.setaxislimits(plotdict)
		labels.add_labels(plotdict)
		if plotdict['save']:
			utils.save(plotdict)

try:
	plot1d = profile(plot1d)
except NameError:
	pass  # not running with profiler, that's ok.


def get_root_histos(plotdict):
	"""Get the ROOT histograms from the files, convert to MPL and add to the dictionary."""
	for rootfile, folder in zip(plotdict["rootfiles"], plotdict['folder']):
		rootobject = getroot.histofromfile(rootfile, plotdict['plot'], folder, plotdict)
		plotdict["roothistos"].append(rootobject)
		plotdict["mplhistos"].append(mplconvert.root2histo(rootobject, rootfile.GetName(), 1))


def plot1d_mpl(plotdict):
	"""Create Matplotlib plots from the ROOT histograms."""

	# use the given fig/axis or create new one:
	if not plotdict['axes'] and not plotdict['figure']:
		figure, ax = utils.newplot()
		plotdict['axes'] = ax
		plotdict['figure'] = figure

	# Scale histos if 'normalize'
	if plotdict['normalize']:
		maxevents = max([h.ysum() for h in plotdict["mplhistos"]])
		for h, r in zip(plotdict["mplhistos"], plotdict["roothistos"]):
			r.Scale(maxevents / h.ysum())
			h.scale(maxevents / h.ysum())

	# iterate over files and plot
	stack = {}
	for histo, marker, label, color, errorbar, group, rootobject in zip(plotdict["mplhistos"], plotdict['markers'],
				plotdict['labels'], plotdict['colors'], plotdict['errorbars'], plotdict['groups'], plotdict['roothistos']):
		if errorbar == "True":
			yerr = histo.yerr
		else:
			yerr = None

		if marker == 'stack':
			if group not in stack:
				if plotdict['ylims'] is not None:
					ybottom = plotdict['ylims'][0]
				elif plotdict['log']:
					ybottom = 1
				else:
					ybottom = 0
				bottom = [ybottom] * len(histo.x)
				stack[group] = bottom
			else:
				bottom = stack[group]
			widths = (histo.x[1] - histo.x[0])
			ax.bar(histo.x, histo.y, widths, bottom=bottom, yerr=yerr, 
					ecolor=color, label=label, fill=True, facecolor=color, edgecolor=color, alpha=0.8)
			stack[group] = [x + y for x, y in zip(histo.y, stack[group])]
		else: 
			ax.errorbar(histo.xc, histo.y, yerr, drawstyle='steps-mid', color=color, fmt=marker, capsize=0, label=label, zorder=10)
		fit.fit(rootobject, plotdict)


def plot1d_root(plotdict):
	"""Create ROOT histograms from ROOT plots."""
	canvas = ROOT.TCanvas()
	plotdict["roothistos"][0].Draw()
	for roothisto in plotdict["roothistos"][1:]:
		roothisto.Draw("same")
	for format in plotdict['formats']:
		canvas.Print("%s.%s" % (plotdict['filename'], format))

