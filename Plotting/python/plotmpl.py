# -*- coding: utf-8 -*-

"""
"""
import os
import pprint
import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)


import HarryPlotter.Plotting.plotbase as plotbase

import HarryPlotter.Utility.mplconvert as mplconvert
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize


class PlotMpl(plotbase.PlotBase):
	def __init__(self):
		super(PlotMpl, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(PlotMpl, self).modify_argument_parser(parser, args)
		self.formatting_options.add_argument("--linestyle", default="-", nargs="+",
		                                     help="Style of errorbar plot line. [Default: '-']")

	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)
		pprint.pprint(plotData.plotdict)
		# defaults for colors
		for index, color in enumerate(plotData.plotdict["colors"]):
			if color == None:
				plotData.plotdict["colors"][index] = "black"
			else:
				plotData.plotdict["colors"][index] = color
		self.set_default_ratio_colors(plotData)
		
		# defaults for markers
		for index, marker in enumerate(plotData.plotdict["markers"]):
			if marker == None:
				plotData.plotdict["markers"][index] = "-"

		self.prepare_list_args(plotData, ["linestyle"], n_items=len(plotData.plotdict["nicks"]))


	def run(self, plotData):
		super(PlotMpl, self).run(plotData)
	
	def create_canvas(self, plotData):
		if plotData.plotdict["ratio"]:
			self.fig = plt.figure(figsize=[5, 7])
			self.ax = self.fig.add_subplot(111, position = [0.13, 0.35, 0.83, 0.58])
			self.ax2 = self.fig.add_subplot(111, position = [0.13, 0.06, 0.83, 0.22], sharex=self.ax)
			#plotdict['ratiosubplotaxes'] = ax2 # needed?
		else:
			self.fig = plt.figure(figsize=[5, 5])
			self.ax = self.fig.add_subplot(111, position = [0.13, 0.10, 0.83, 0.83])

		# place labels specified from command-line
		plt.text(0.5, 1.035, plotData.plotdict["title"], horizontalalignment='center', transform=self.ax.transAxes, fontsize=14)
		if not (plotData.plotdict["lumi"]==None):
			plt.text(-0.0, 1.030, "$\mathcal{L}=" + str(plotData.plotdict["lumi"]) + "\mathrm{fb^{-1}}$", transform=self.ax.transAxes, fontsize=10)
		if not (plotData.plotdict["energy"] == None):
			energy = "+".join(plotData.plotdict["energy"])
			plt.text(1.0, 1.030, "$\sqrt{s}=" + energy + "\\ \mathrm{TeV}$", transform=self.ax.transAxes, fontsize=10, horizontalalignment="right")

		if plotData.plotdict["x_lims"] != None:
			self.ax.set_xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
		if plotData.plotdict["y_lims"] != None:
			self.ax.set_ylim(plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1])

		if plotData.plotdict["ratio"] and plotData.plotdict["y_ratio_lims"] != None:
			self.ax2.set_ylim(plotData.plotdict["y_ratio_lims"][0],plotData.plotdict["y_ratio_lims"][1])

	def prepare_histograms(self, plotData):
		# TODO: summation is now done in upper class
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

		for nick, color, label, marker, errorbar, stack, linestyle in zip(plotData.plotdict["nicks"],
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"],
		                                 plotData.plotdict["errorbars"],
		                                 plotData.plotdict["stack"],
		                                 plotData.plotdict["linestyle"]):
			root_histogram = plotData.plotdict["root_histos"][nick]
			mpl_histogram = mplconvert.root2histo(root_histogram, "someFilename", 1)
			self.plot1D = isinstance(mpl_histogram, mplconvert.Histo)
                        self.plot2D = isinstance(mpl_histogram, mplconvert.Histo2D)

			# convert linestyles that do not work on command line
			if linestyle == "dotted":
				linestyle = "--"

			# determine bin width to allow variable binning
			widths = []
			for i in range(root_histogram.GetNbinsX()):
				widths.append(root_histogram.GetBinWidth(i))
			yerr=mpl_histogram.yerr if errorbar else None

			if stack in self.bottom_y_values:
				bottom = self.bottom_y_values[stack]
				self.bottom_y_values[stack] =  [sum(x) for x in zip(self.bottom_y_values[stack], mpl_histogram.y)]
			else:
				bottom = [0] * root_histogram.GetNbinsX()
			# do the actual plotting
			if self.plot2D:
				norm = LogNorm if plotData.plotdict["z_log"] else Normalize
				self.image = self.ax.imshow(mpl_histogram.BinContents,
                                               interpolation='nearest',
				               origin='lower',
				               extent=[mpl_histogram.xborderlow, mpl_histogram.xborderhigh, mpl_histogram.yborderlow, mpl_histogram.yborderhigh],
				               aspect='auto',
				               cmap=plt.cm.get_cmap('afmhot'),
				               norm=norm())
			elif marker=="bar":
				self.ax.bar(mpl_histogram.x, mpl_histogram.y, widths, yerr=yerr, bottom=bottom,
				            ecolor=color, label=label, fill=True, facecolor=color, edgecolor=color, alpha=0.8)
			else:
				y = mpl_histogram.y if not(stack in self.bottom_y_values) else self.bottom_y_values[stack]
					
				self.ax.errorbar(mpl_histogram.xc, y, mpl_histogram.yerr,
				                 color=color, fmt=marker, capsize=0, label=label, zorder=10, drawstyle='steps-mid', linestyle=linestyle)
		if plotData.plotdict["ratio"]:
			for root_histogram, ratio_color, ratio_marker, in zip(plotData.plotdict["root_ratio_histos"],
			                                      plotData.plotdict["ratio_colors"],
			                                      plotData.plotdict["ratio_markers"]):
				mpl_histogram = mplconvert.root2histo(root_histogram, "someFilename", 1)
				self.ax2.errorbar(mpl_histogram.xc, mpl_histogram.y, mpl_histogram.yerr, ecolor=ratio_color, fmt=ratio_marker)


	def modify_axes(self, plotData):
		# do what is needed for all plots:
		super(PlotMpl, self).modify_axes(plotData)

		self.ax.grid(plotData.plotdict["grid"])

		self.ax.set_xlabel(plotData.plotdict["x_label"])
		self.ax.set_ylabel(plotData.plotdict["y_label"])
		self.ax.ticklabel_format(style='sci',scilimits=(-3,4),axis='both')

		# do special things for 1D Plots
		if self.plot1D:
			if plotData.plotdict["x_log"]: 
				self.ax.set_xscale('log', nonposx='mask')
			if plotData.plotdict["y_log"]: 
				self.ax.set_yscale('log', nonposx='mask')

			if plotData.plotdict["ratio"]:
				self.ax2.set_xlabel(plotData.plotdict["x_label"])
				self.ax2.set_ylabel(plotData.plotdict["y_ratio_label"])
				self.ax2.grid(plotData.plotdict["ratio_grid"])

		# do special things for 2D Plots
		if self.plot2D:
			cb = self.fig.colorbar(self.image, ax=self.ax)


	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)

		legend = self.ax.legend()

	def save_canvas(self, plotData):
		for output_filename in plotData.plotdict["output_filenames"]:
			self.fig.savefig(output_filename)
