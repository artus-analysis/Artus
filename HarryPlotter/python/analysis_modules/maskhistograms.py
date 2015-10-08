# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase


class MaskHistograms(histogrammanipulationbase.HistogramManipulationBase):
	"""Mask areas of histograms."""
	
	def modify_argument_parser(self, parser, args):
		super(MaskHistograms, self).modify_argument_parser(parser, args)
		
		self.MaskHistograms_options = parser.add_argument_group(self.name() + " options")
		self.MaskHistograms_options.add_argument(
				"--mask-histogram-nicks", type=str, nargs="+", default=[],
				help="List the nicks you want to use (-> module whitelist ). [Default: all]"
		)
		self.MaskHistograms_options.add_argument(
				"--mask-above-delta-min", type=float, default=1.0,
				help="Lower theshold on (bin content) - (min. bin content). [Default: %(default)s]"
		)
	
	def prepare_args(self, parser, plotData):
		super(MaskHistograms, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["mask_histogram_nicks"]) > 0:
			self.whitelist = plotData.plotdict["mask_histogram_nicks"]
		else:
			self.whitelist = plotData.plotdict["nicks"]
	
	def _selector(self, nick, root_histogram, plotData):
		if not isinstance(root_histogram, ROOT.TH1):
			return False
		
		self.mask_above_delta_min = plotData.plotdict["mask_above_delta_min"]
		self.histogram_min = root_histogram.GetMinimum()
		return super(MaskHistograms, self)._selector(nick, root_histogram, plotData)
	
	def _manipulate_bin(self, histogram, global_bin):
		if histogram.GetBinContent(global_bin) - self.histogram_min > self.mask_above_delta_min:
			histogram.SetBinContent(global_bin, 0.0)
			histogram.SetBinError(global_bin, 0.0)

