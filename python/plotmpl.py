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
		pprint.pprint(plotData.plotdict)
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

		plt.title(plotData.plotdict["title"])

		if plotData.plotdict["x_lims"] != None:
			plt.xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
		if plotData.plotdict["y_lims"] != None:
			plt.ylim(plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1])

	def prepare_histograms(self, plotData):
		# create a dictionary with one entry for each plot that will be stacked
		self.bottom_y_values = {}
		for stack in plotData.plotdict["stack"]:
			if plotData.plotdict["stack"].count(stack) > 1:
				histo_size = plotData.plotdict["root_histos"]["nick0"].GetNbinsX()
				if not (stack in self.bottom_y_values):
					self.bottom_y_values[stack] = [0] * histo_size
				else:
					if not (len(self.bottom_y_values[stack]) == histo_size):
						print "histograms labled with --stack " + stack + " do not have the same size and can not be stacked"

	def make_plots(self, plotData):

		#plotData.plotdict["root_histos"]

		for nick, color, label, marker, errorbar, stack in zip(plotData.plotdict["nicks"],
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"],
		                                 plotData.plotdict["errorbars"],
		                                 plotData.plotdict["stack"]):
			root_histogram = plotData.plotdict["root_histos"][nick]
			mpl_histogram = mplconvert.root2histo(root_histogram, "someFilename", 1)

			# determine bin width to allow variable binning
			widths = []
			for i in range(root_histogram.GetNBinsX()):
				widths.append(root_histogram.GetBinWidth(i))
			yerr=mpl_histogram.yerr if errorbar else None

			if stack in self.bottom_y_values:
				bottom = self.bottom_y_values[stack]
				self.bottom_y_values[stack] =  [sum(x) for x in zip(self.bottom_y_values[stack], mpl_histogram.y)]

			if marker=="bar":
				self.ax.bar(mpl_histogram.x, mpl_histogram.y, widths, yerr=yerr, bottom=bottom,
				            ecolor=color, label=label, fill=True, facecolor=color, edgecolor=color, alpha=0.8)
			else:
				y = mpl_histogram.y if not(stack in self.bottom_y_values) else self.bottom_y_values[stack]
			
				self.ax.errorbar(mpl_histogram.xc, y, mpl_histogram.yerr,
				                 color=color, fmt=marker, capsize=0, label=label, zorder=10, drawstyle='steps-mid', linestyle="-.")

	def modify_axes(self, plotData):
		super(PlotMpl, self).modify_axes(plotData)

		plt.grid(plotData.plotdict["grid"])

		plt.xlabel(plotData.plotdict["x_label"])
		plt.ylabel(plotData.plotdict["y_label"])

		if plotData.plotdict["x_log"]: 
			self.ax.set_xscale('log', nonposx='mask')

		if plotData.plotdict["y_log"]: 
			self.ax.set_yscale('log', nonposx='mask')


	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)
	
		self.ax.legend()

	def save_canvas(self, plotData):
		for plot_format in plotData.plotdict["formats"]:
			filename = os.path.join(plotData.plotdict["output_dir"],
				                     plotData.plotdict["filename"]+"."+plot_format)
			self.fig.savefig(filename)
