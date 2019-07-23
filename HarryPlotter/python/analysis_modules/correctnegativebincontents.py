# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase


class CorrectNegativeBinContents(histogrammanipulationbase.HistogramManipulationBase):
	"""Set bin errors of negative bins to 0 while preserving the integral of histograms."""

	def __init__(self):
		super(CorrectNegativeBinContents, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(CorrectNegativeBinContents, self).modify_argument_parser(parser, args)

		self.correctnegbincontent_options = parser.add_argument_group("CorrectNegativeBinContents options")
		self.correctnegbincontent_options.add_argument(
				"--nicks-correct-negative-bins", nargs="+", default=[],
				help="Nicks of histograms to be corrected. [Default: all]"
		)
		self.correctnegbincontent_options.add_argument(
				"--no-integral-preservation", nargs="+", type="bool", default=False,
				help="Do not preserve integral. [Default: %(default)s]]"
		)
	
	def prepare_args(self, parser, plotData):
		super(CorrectNegativeBinContents, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["nicks_correct_negative_bins"]) > 0:
			self.whitelist = plotData.plotdict["nicks_correct_negative_bins"]
		else:
			self.whitelist = plotData.plotdict["nicks"]
	
		if plotData.plotdict["no_integral_preservation"] == False:
			self.mode = "preserve_integral"
		else:
			self.mode = "do_not_preserve_integral"
	
	def _selector(self, nick, root_histogram, plotData):
		if isinstance(root_histogram, ROOT.TH1):
			self.original_integral = root_histogram.Integral()
		else:
			return False
		return super(CorrectNegativeBinContents, self)._selector(nick, root_histogram, plotData)
	
	def _manipulate_bin(self, histogram, global_bin):
		if histogram.GetBinContent(global_bin) < 0.0:
			histogram.SetBinContent(global_bin, 0.0)
			
			# preserve original integral
			if self.mode == "preserve_integral":
				new_integral = histogram.Integral()
				if new_integral > 0.0:
					histogram.Scale(abs(self.original_integral / new_integral))

