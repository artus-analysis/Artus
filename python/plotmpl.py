# -*- coding: utf-8 -*-

"""
"""
import os
import pprint
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


import Artus.HarryPlotter.plotbase as plotbase

import Artus.HarryPlotter.tools.mplconvert as mplconvert
import matplotlib.pyplot as plt

class PlotMpl(plotbase.PlotBase):
	def __init__(self):
		super(PlotMpl, self).__init__()
	
	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)
		
		# defaults for colors
		for index, color in enumerate(plotData.plotdict["colors"]):
			pass # TODO
		self.set_default_ratio_colors(plotData)
	
	def run(self, plotData):
		super(PlotMpl, self).run(plotData)
	
	def create_canvas(self, plotData):
		if plotData.plotdict["ratio"]:
			self.fig = plt.figure(figsize=[7, 7])
			self.ax1 = self.fig.add_subplot(111, position = [0.13, 0.37, 0.83, 0.58])
			self.ax2 = self.fig.add_subplot(111, position = [0.13, 0.12, 0.83, 0.22], sharex=self.ax1)
			#plotdict['ratiosubplotaxes'] = ax2 # needed?
		else:
			self.fig = plt.figure(figsize=[7, 7])
			self.ax = self.fig.add_subplot(111)

	def prepare_histograms(self, plotData):
		# create root histograms with TTree:Project

		# normalize if wanted
		# call functions from plotroot?
		pass

	def make_plots(self, plotData):
		print "in MakePlots"
		print plotData.plotdict["root_histos"]
		pprint.pprint(plotData.plotdict)
		plotData.plotdict["root_histos"]
		for root_histogram in plotData.plotdict["root_histos"].values(): # todo: filename, color
			mpl_histogram = mplconvert.root2histo(root_histogram, "someFilename", 1)
		label = "dummylabel"
		color = "blue"
		marker = "-"
		self.ax.errorbar(mpl_histogram.xc, mpl_histogram.y, mpl_histogram.yerr, color=color, fmt=marker, capsize=0, label=label, zorder=10, drawstyle='default')


	def save_canvas(self, plotData):
		for plot_format in plotData.plotdict["formats"]:
			filename = os.path.join(plotData.plotdict["output_dir"],
				                     plotData.plotdict["filename"]+"."+plot_format)
			self.fig.savefig(filename)
