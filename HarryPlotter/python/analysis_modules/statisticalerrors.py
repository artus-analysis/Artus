# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase

class StatisticalErrors(histogrammanipulationbase.HistogramManipulationBase):
	"""Replace bin contents of selected histograms with bin errors."""
	def __init__(self):
		super(StatisticalErrors, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(StatisticalErrors, self).modify_argument_parser(parser, args)

		self.StatisticalErrors_options = parser.add_argument_group("StatisticalErrors options")
		self.StatisticalErrors_options.add_argument("--stat-error-nicks", type=str, nargs="+",
				help="List the nicks you want to make stat error (-> module whitelist ).")
		self.StatisticalErrors_options.add_argument("--stat-error-relative", action="store_true", default=False,
				help="Do not plot absolute errors, but relative to bin content. [Default: %(default)s]")
		self.StatisticalErrors_options.add_argument("--stat-error-relative-percent", action="store_true", default=False,
				help="Output relative errors as percentage. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(StatisticalErrors, self).prepare_args(parser, plotData)
		self.whitelist = plotData.plotdict["stat_error_nicks"]
		self.relative_error = plotData.plotdict["stat_error_relative"]
		self.relative_error_percent = plotData.plotdict["stat_error_relative_percent"]

	def _manipulate_bin(self, histogram, global_bin):
		new_bin_content = histogram.GetBinError(global_bin)
		if self.relative_error:
			content = histogram.GetBinContent(global_bin)
			if content > 0:
				new_bin_content /= content
				if self.relative_error_percent:
					new_bin_content *= 100.0
			else:
				new_bin_content = 0
		histogram.SetBinContent(global_bin, new_bin_content)
		histogram.SetBinError(global_bin, 0)
