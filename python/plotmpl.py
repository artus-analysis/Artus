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

	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)
		
		# defaults for colors
		for index, color in enumerate(plotData.plotdict["colors"]):
			if color == None:
				plotData.plotdict["colors"][index] = index + 1
			else:
				plotData.plotdict["colors"][index] = color
		#self.set_default_ratio_colors(plotData)
		
		# defaults for markers
		for index, marker in enumerate(plotData.plotdict["markers"]):
			if marker == None:
				plotData.plotdict["markers"][index] = "-"


	def run(self, plotData):
		super(PlotMpl, self).run(plotData)
	
	def create_canvas(self, plotData):
		if plotData.plotdict["ratio"]:
			self.fig = plt.figure(figsize=[4, 4])
			self.ax1 = self.fig.add_subplot(111, position = [0.13, 0.37, 0.83, 0.58])
			self.ax2 = self.fig.add_subplot(111, position = [0.13, 0.12, 0.83, 0.22], sharex=self.ax1)
			#plotdict['ratiosubplotaxes'] = ax2 # needed?
		else:
			self.fig = plt.figure(figsize=[7, 7])
			self.ax = self.fig.add_subplot(111)

	def prepare_histograms(self, plotData):
		pass

	def make_plots(self, plotData):
		print "in MakePlots"
		pprint.pprint(plotData.plotdict)
		plotData.plotdict["root_histos"]

		for root_histogram, color, label, marker in zip(plotData.plotdict["root_histos"].values(),
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"]):
			mpl_histogram = mplconvert.root2histo(root_histogram, "someFilename", 1)
			self.ax.errorbar(mpl_histogram.xc, mpl_histogram.y, mpl_histogram.yerr, color=color, fmt=marker, capsize=0, label=label, zorder=10, drawstyle='steps-mid')

	def modify_axes(self, plotData):
		super(PlotMpl, self).modify_axes(plotData)
		plt.grid(plotData.plotdict["grid"])

		plt.xlabel(plotData.plotdict["x_label"])
		plt.ylabel(plotData.plotdict["y_label"])

		if plotData.plotdict["y_log"]: 
			self.ax.set_xscale('log', nonposx='mask')

		if plotData.plotdict["y_log"]: 
			self.ax.set_yscale('log', nonposx='mask')

		plt.title(plotData.plotdict["title"])

		print plotData.plotdict["x_lims"]
		if plotData.plotdict["x_lims"] != None:
			plt.xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
		if plotData.plotdict["y_lims"] != None:
			plt.ylim([plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1]])

	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)
	
		self.ax.legend()

	def save_canvas(self, plotData):
		for plot_format in plotData.plotdict["formats"]:
			filename = os.path.join(plotData.plotdict["output_dir"],
				                     plotData.plotdict["filename"]+"."+plot_format)
			self.fig.savefig(filename)
