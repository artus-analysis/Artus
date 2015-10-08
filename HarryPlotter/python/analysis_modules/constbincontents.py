# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase


class ConstBinContentsMin(histogrammanipulationbase.HistogramManipulationBase):
	"""Set bin contents of non-empty bins to the minimum bin content of the histogram."""
	
	def modify_argument_parser(self, parser, args):
		super(ConstBinContentsMin, self).modify_argument_parser(parser, args)
		
		self.ConstBinContentsMin_options = parser.add_argument_group(self.name() + " options")
		self.ConstBinContentsMin_options.add_argument(
				"--const-bin-contents-nicks", type=str, nargs="+", default=[],
				help="List the nicks you want to use (-> module whitelist ). [Default: all]"
		)
	
	def prepare_args(self, parser, plotData):
		super(ConstBinContentsMin, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["const_bin_contents_nicks"]) > 0:
			self.whitelist = plotData.plotdict["const_bin_contents_nicks"]
		else:
			self.whitelist = plotData.plotdict["nicks"]
	
	def _selector(self, nick, root_histogram, plotData):
		if not isinstance(root_histogram, ROOT.TH1):
			return False
		
		self.histogram_min = root_histogram.GetMinimum(0.0)
		return super(ConstBinContentsMin, self)._selector(nick, root_histogram, plotData)
	
	def _manipulate_bin(self, histogram, global_bin):
		if histogram.GetBinContent(global_bin) != 0.0:
			histogram.SetBinContent(global_bin, self.histogram_min)
			histogram.SetBinError(global_bin, 0.0)

