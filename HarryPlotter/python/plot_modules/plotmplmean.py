# -*- coding: utf-8 -*-

"""
	Plot a vertical line at the x-mean of the root object.

	Basic usage: harry.py HARRY-ARGS --plot-modules PlotMpl PlotMplMean
"""

import Artus.HarryPlotter.plotbase as plotbase


class PlotMplMean(plotbase.PlotBase):

	def modify_argument_parser(self, parser, args):
		#TODO add argument for x or y mean
		#TODO make alpha, linewidth/-style etc. configurable
		#TODO make configurable which nicks are used
		pass

	def prepare_args(self, parser, plotData):
		pass

	def run(self, plotData):
		for nick, color in zip(plotData.plotdict['nicks'], plotData.plotdict['colors']):
			mean = plotData.plotdict['root_objects'][nick].GetMean()
			mean_err = plotData.plotdict['root_objects'][nick].GetMeanError()
			for ax in plotData.plot.axes[:1]:
				ax.axvline(mean, color=color)
				ax.axvspan(mean-mean_err, mean+mean_err, color=color, alpha=0.2, linewidth=0)
