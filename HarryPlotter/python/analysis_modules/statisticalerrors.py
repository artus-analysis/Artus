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
		self.StatisticalErrors_options.add_argument("--stat-error", type=str, nargs='*', default=[],
				help="List the nicks you want to make stat error (-> module whitelist ).")
		self.StatisticalErrors_options.add_argument("--relative-error", action="store_true", default=False,
				help="Do not plot absolute errors, but relative to bin content (in %).")

	def prepare_args(self, parser, plotData):
		super(StatisticalErrors, self).prepare_args(parser, plotData)
		self.whitelist = plotData.plotdict['stat_error']
		self.relative_error = plotData.plotdict['relative_error']

	def _manipulate_bin(self, histogram, global_bin):
		new_bin_content = histogram.GetBinError(global_bin)
		if self.relative_error:
			content = histogram.GetBinContent(global_bin)
			if content > 0:
				new_bin_content *= (100./content)
			else:
				new_bin_content = 0
		histogram.SetBinContent(global_bin, new_bin_content)
		histogram.SetBinError(global_bin, 0)
