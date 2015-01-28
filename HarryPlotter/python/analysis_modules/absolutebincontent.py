# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase

class AbsoluteBinContents(histogrammanipulationbase.HistogramManipulationBase):
	"""Replace bin contents of selected histograms with absolute bin contents"""

	def modify_argument_parser(self, parser, args):
		super(AbsoluteBinContents, self).modify_argument_parser(parser, args)

		self.AbsoluteBinContents_options = parser.add_argument_group("AbsoluteBinContents options")
		self.AbsoluteBinContents_options.add_argument("--absolute-bin-contents", type=str, nargs='*', default=[],
				help="List the nicks you want to make absolute (-> module whitelist ).")

	def prepare_args(self, parser, plotData):
		super(AbsoluteBinContents, self).prepare_args(parser, plotData)
		self.whitelist = plotData.plotdict['absolute_bin_contents']

	def _manipulate_bin(self, histogram, global_bin):
		histogram.SetBinContent(global_bin, abs(histogram.GetBinContent(global_bin)))

