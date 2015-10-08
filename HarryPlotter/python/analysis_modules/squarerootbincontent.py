# -*- coding: utf-8 -*-

import math

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase

class SquareRootBinContent(histogrammanipulationbase.HistogramManipulationBase):
	"""Replace bin contents of selected histograms with square root of bin contents"""

	def modify_argument_parser(self, parser, args):
		super(SquareRootBinContent, self).modify_argument_parser(parser, args)

		self.SquareRootBinContent_options = parser.add_argument_group(self.__class__.__name__ + " options")
		self.SquareRootBinContent_options.add_argument("--square-root-nicks", type=str, nargs='+',
				help="List the nicks you want to use (-> module whitelist ).")

	def prepare_args(self, parser, plotData):
		super(SquareRootBinContent, self).prepare_args(parser, plotData)
		self.whitelist = plotData.plotdict['square_root_nicks']

	def _manipulate_bin(self, histogram, global_bin):
		histogram.SetBinContent(global_bin, math.sqrt(histogram.GetBinContent(global_bin)))

