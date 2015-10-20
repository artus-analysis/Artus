# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import math

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase


class FillEmptyHistograms(histogrammanipulationbase.HistogramManipulationBase):
	"""Fill empty histograms with negligible contents in all bins."""

	def __init__(self):
		super(FillEmptyHistograms, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(FillEmptyHistograms, self).modify_argument_parser(parser, args)

		self.fill_empty_histograms_options = parser.add_argument_group("FillEmptyHistograms options")
		self.fill_empty_histograms_options.add_argument(
				"--nicks-fill-empty-histograms", nargs="+", default=[],
				help="Nicks of histograms to be filled. [Default: all]"
		)
		self.fill_empty_histograms_options.add_argument(
				"--fill-empty-histograms-integral", default=1e-5,
				help="Integral for filled histograms. [Default: %(default)s]"
		)
	
	def prepare_args(self, parser, plotData):
		super(FillEmptyHistograms, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["nicks_fill_empty_histograms"]) > 0:
			self.whitelist = plotData.plotdict["nicks_fill_empty_histograms"]
		else:
			self.whitelist = plotData.plotdict["nicks"]
	
	def _selector(self, nick, root_histogram, plotData):
		if isinstance(root_histogram, ROOT.TH1) and (root_histogram.Integral() == 0.0):
			self.bin_content = plotData.plotdict["fill_empty_histograms_integral"] / (root_histogram.GetNbinsX()*root_histogram.GetNbinsY()*root_histogram.GetNbinsZ())
			self.bin_error = math.sqrt(self.bin_content)
			return super(FillEmptyHistograms, self)._selector(nick, root_histogram, plotData)
		else:
			return False
	
	def _manipulate_bin(self, histogram, global_bin):
		histogram.SetBinContent(global_bin, self.bin_content)
		histogram.SetBinError(global_bin, self.bin_error)

