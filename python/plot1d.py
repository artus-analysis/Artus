# -*- coding: utf-8 -*-

#### todo: adjust to dictionary plotdict to save root files
#### todo: allow interaction with external modules
#### todo: save root histograms for import/export
#### todo: save command-line settings to reproduce the plot later/ plotfromdict
#### todo: enable ratiosubplot

"""Plot quantities from data and MC.

	This module is dedicated to the creating 1D plots.
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import numpy
import math
import ROOT
import sys
import os
import copy
import matplotlib.pyplot as plt

import Artus.HarryPlotter.tools.getroot as getroot
import Artus.HarryPlotter.tools.mplconvert as mplconvert
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.tools.fit as fit


def plot1d(plotdict):
	"""Function for all data/MC comparison plots for basic quantities."""

	log.info("A 1D %s plot is created" % (plotdict["plot"]))

	get_root_histos(plotdict)

	if plotdict['backend'] == 'root':
		plot1d_root(plotdict)
	elif plotdict['backend'] == 'mpl':
		get_mpl_histos(plotdict)
		plot1d_mpl(plotdict)
		if plotdict['ratiosubplot']:
			add_ratiosubplot(plotdict)
		utils.setaxislimits(plotdict)
		labels.add_labels(plotdict)
		if plotdict['save']:
			utils.save(plotdict)

try:
	plot1d = profile(plot1d)
except NameError:
	pass  # not running with profiler, that's ok.


def get_root_histos(plotdict):
	"""Get the ROOT histograms from the files and add to the dictionary."""
	for rootfile, folder in zip(plotdict["rootfiles"], plotdict['folder']):
		rootobject = getroot.histofromfile(rootfile, plotdict['plot'], folder, plotdict)
		plotdict["roothistos"].append(rootobject)

		if plotdict.get('scale', None) is not None:
			rootobject.Scale(plotdict['scale']/rootobject.Integral())
		if plotdict.get('scalefactor', None) is not None:
			rootobject.Scale(plotdict['scalefactor'])

		if log.isEnabledFor(logging.DEBUG):
			rootobject.Print()

def get_mpl_histos(plotdict):
	"""Convert ROOT histograms to MPL and add to the dictionary."""
	for roothisto, rootfile in zip(plotdict["roothistos"], plotdict['rootfiles']):
		plotdict["mplhistos"].append(mplconvert.root2histo(roothisto, rootfile.GetName(), 1))


def plot1d_mpl(plotdict):
	"""Create Matplotlib plots from the MPL histograms."""

	# use the given fig/axis or create new one:
	if not plotdict['axes'] and not plotdict['figure']:
		figure, ax = utils.newplot(plotdict)
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
		if errorbar == True:
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
			ax.errorbar(histo.xc, histo.y, yerr, color=color, fmt=marker, capsize=0, label=label, zorder=10,
			            drawstyle='default' if "TGraph" in histo.classname else 'steps-mid')
		fit.fit(rootobject, plotdict)

def add_ratiosubplot(plotdict):
	"""This function adds the ratio of the first two root objects to a subplot."""

	rootratio = getroot.rootdivision(plotdict["roothistos"])
	mplratio = mplconvert.root2histo(rootratio, "Ratio",)

	plotdict['ratiosubplotaxes'].errorbar(mplratio.xc, mplratio.y, mplratio.yerr,
		drawstyle='steps-mid', color='black', fmt='o', capsize=0, label="Ratio")

	# Some formatting:
	plotdict['ratiosubplotaxes'].set_ylabel("%s/%s ratio" % (plotdict['labels'][0],
			plotdict['labels'][1]) )
	plotdict['ratiosubplotaxes'].axhline(1.0, color='gray', lw=1)
	plt.setp(plotdict['axes'].get_xticklabels(), visible=False)

	# Set custom ylim if given, if not fall back to [0.5, 1.5] 
	if plotdict['ylims'] is not None and len(plotdict['ylims']) > 2:
		plotdict['ratiosubplotaxes'].set_ylim(plotdict['ylims'][2], plotdict['ylims'][3])
	else:
		plotdict['ratiosubplotaxes'].set_ylim(0.5, 1.5)
		
	# This should be done automatically by MPL because of the shared axis;
	# I dont know why it doesnt:
	if plotdict['xlog']: 
		plotdict['ratiosubplotaxes'].set_xscale('log', nonposx='mask')

	if plotdict['grid']:
		plotdict['ratiosubplotaxes'].grid(True)


def plot1d_root(plotdict):
	"""Create ROOT histograms from ROOT plots."""
	canvas = ROOT.TCanvas()
	plotdict["roothistos"][0].Draw()
	for roothisto in plotdict["roothistos"][1:]:
		roothisto.Draw("same")
	for format in plotdict['formats']:
		canvas.Print("%s.%s" % (plotdict['filename'], format))

