# -*- coding: utf-8 -*-

"""
	Plot a vertical line at the x-mean of the root object.

	Basic usage: harry.py HARRY-ARGS --plot-modules PlotMpl PlotMplMean
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase


class PlotMplMean(plotbase.PlotBase):

	def modify_argument_parser(self, parser, args):
		#TODO add argument for x or y mean
		#TODO make alpha, linewidth/-style etc. configurable
		self.mean_options = parser.add_argument_group(self.__class__.__name__ + " options")
		self.mean_options.add_argument("--mean-nicks", nargs="*", default=None,
		                               help="Whitelist of nick names for this module. If not given, use all.")

	def prepare_args(self, parser, plotData):
		pass

	def run(self, plotData):
		if plotData.plotdict['mean_nicks'] is None:
			nicks = plotData.plotdict['nicks']
		else:
			nicks = plotData.plotdict['mean_nicks']
		for nick, color in zip(nicks, plotData.plotdict['colors']):
			try:
				mean = plotData.plotdict['root_objects'][nick].GetMean()
				mean_err = plotData.plotdict['root_objects'][nick].GetMeanError()
			except AttributeError:
				log.warning("Could not get mean for nick", nick)
				continue
			for ax in plotData.plot.axes[:1]:
				ax.axvline(mean, color=color)
				ax.axvspan(mean-mean_err, mean+mean_err, color=color, alpha=0.2, linewidth=0)
