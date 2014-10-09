# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase


class BinErrorsOfEmptyBins(analysisbase.AnalysisBase):
	def __init__(self):
		super(BinErrorsOfEmptyBins, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(BinErrorsOfEmptyBins, self).modify_argument_parser(parser, args)
		
		self.empty_bins_options = parser.add_argument_group("Empty bins errors options")
		self.empty_bins_options.add_argument("--empty-bin-error-scale", type=float, default=0.6,
				help="Scale factor for bin errors of empty bins = sf * sum of weights / entries of histogram. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(BinErrorsOfEmptyBins, self).prepare_args(parser, plotData)
		
	
	def run(self, plotData=None):
		super(BinErrorsOfEmptyBins, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1):
				bin_error_for_empty_bins = plotData.plotdict["empty_bin_error_scale"] * root_histogram.GetSumOfWeights() / root_histogram.GetEntries()
			
				for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
					for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
						for z_bin in xrange(1, root_histogram.GetNbinsY()+1):
							global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
							if root_histogram.GetBinContent(global_bin) == 0.0:
								root_histogram.SetBinError(global_bin, bin_error_for_empty_bins)

