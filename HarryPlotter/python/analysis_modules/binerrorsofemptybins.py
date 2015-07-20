# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase

class BinErrorsOfEmptyBins(histogrammanipulationbase.HistogramManipulationBase):
	"""Missing Documentation"""
	
	def modify_argument_parser(self, parser, args):
		super(BinErrorsOfEmptyBins, self).modify_argument_parser(parser, args)
		
		self.empty_bins_options = parser.add_argument_group("Empty bins errors options")
		self.empty_bins_options.add_argument("--empty-bin-error-scale", type=float, default=0.6,
				help="Scale factor for bin errors of empty bins = sf * sum of weights / entries of histogram. [Default: %(default)s]")

	def _selector(self, nick, root_histogram, plotData):
		if isinstance(root_histogram, ROOT.TH1):
			self.bin_error_for_empty_bins = plotData.plotdict["empty_bin_error_scale"] * root_histogram.GetSumOfWeights() / root_histogram.GetEntries()
		return super(BinErrorsOfEmptyBins, self)._selector(nick, root_histogram, plotData)

	def _manipulate_bin(self, histogram, global_bin):
		if root_histogram.GetBinContent(global_bin) == 0.0:
			root_histogram.SetBinError(global_bin, self.bin_error_for_empty_bins)
