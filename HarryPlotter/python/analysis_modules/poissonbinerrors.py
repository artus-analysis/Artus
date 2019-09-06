# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import math

import ROOT

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase


class PoissonBinErrors(histogrammanipulationbase.HistogramManipulationBase):
	"""Set bin errors of histograms to sqrt(bin content)."""
	
	def __init__(self):
		super(PoissonBinErrors, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(PoissonBinErrors, self).modify_argument_parser(parser, args)

		self.poisson_bin_errors_options = parser.add_argument_group("{} options".format(self.name()))
		self.poisson_bin_errors_options.add_argument("--poisson-bin-errors-nicks", nargs="+", default=None,
				help="Nick names of the histograms where bin errors should become Poissonian. [Default: all]")

	def prepare_args(self, parser, plotData):
		super(PoissonBinErrors, self).prepare_args(parser, plotData)
		if plotData.plotdict["poisson_bin_errors_nicks"] is None:
			plotData.plotdict["poisson_bin_errors_nicks"] = plotData.plotdict["nicks"]
	
	def _selector(self, nick, root_histogram, plotData):
		return (nick in plotData.plotdict["poisson_bin_errors_nicks"]) and isinstance(root_histogram, ROOT.TH1)

	def _manipulate_bin(self, root_histogram, global_bin):
		root_histogram.SetBinError(global_bin, math.sqrt(abs(root_histogram.GetBinContent(global_bin))))

