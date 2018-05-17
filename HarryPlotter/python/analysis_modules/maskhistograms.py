# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase
from operator import xor

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
				"--mask-above-delta-min", type=float, default=None,
				help="Lower theshold on (bin content) - (min. bin content)."
		)
		self.MaskHistograms_options.add_argument(
				"--mask-above-reference-nick", type=str, default=None,
				help="Nickname of the reference histogram."
		)
		self.MaskHistograms_options.add_argument(
				"--mask-above-reference-value", type=float, default=None,
				help="Mask if reference histogram is above this value, e.g.  s/sqrt(b)."
		)
		self.MaskHistograms_options.add_argument(
				"--mask-below-threshold", type=float, default=None,
				help="Mask if reference histogram is below this threshold, e.g.  mT<50."
		)	
	def prepare_args(self, parser, plotData):
		super(MaskHistograms, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["mask_histogram_nicks"]) > 0:
			self.whitelist = plotData.plotdict["mask_histogram_nicks"]
		else:
			self.whitelist = plotData.plotdict["nicks"]
		# if not any(plotData.plotdict[option] != Noneshold):
		# 	log.fatal("invalid options selected. MaskHistograms is not configured properly. Either both nick and delta mode are selected or none")
		# 	import sys
		# 	sys.exit()
		if(plotData.plotdict["mask_above_delta_min"] != None):
			self.mode = "delta"
		elif(plotData.plotdict["mask_below_threshold"] != None):
			self.mode = "by_upper_threshold"
			self.reference_value = 	plotData.plotdict["mask_below_threshold"]
		else:
			self.mode = "reference"
			self.reference_value = plotData.plotdict["mask_above_reference_value"]

	def run(self, plotData=None):
		if(self.mode == "reference"):
			self.reference_histogram = plotData.plotdict["root_objects"][plotData.plotdict["mask_above_reference_nick"]]
		super(MaskHistograms, self).run(plotData)

	def _selector(self, nick, root_histogram, plotData):
		if not isinstance(root_histogram, ROOT.TH1):
			return False
		
		self.mask_above_delta_min = plotData.plotdict["mask_above_delta_min"]
		self.histogram_min = root_histogram.GetMinimum()
		return super(MaskHistograms, self)._selector(nick, root_histogram, plotData)
	
	def _manipulate_bin(self, histogram, global_bin):
		if(self.mode == "delta"):
			if histogram.GetBinContent(global_bin) - self.histogram_min > self.mask_above_delta_min:
				histogram.SetBinContent(global_bin, 0.0)
				histogram.SetBinError(global_bin, 0.0)
		elif(self.mode == "reference"):
			if self.reference_histogram.GetBinContent(global_bin) > self.reference_value:
				histogram.SetBinContent(global_bin, 0.0)
				histogram.SetBinError(global_bin, 0.0)
		elif(self.mode == "by_upper_threshold"):
			if (histogram.GetBinCenter(global_bin) + histogram.GetBinWidth(global_bin) / 2 ) < self.reference_value:
				histogram.SetBinContent(global_bin, 0.0)
				histogram.SetBinError(global_bin, 0.0)				
		else:
			log.error("invalid mode selected")
