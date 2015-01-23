# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class StatisticalErrors(analysisbase.AnalysisBase):
	"""Plot statistical errors instead of actual bin contents."""
	def __init__(self):
		super(StatisticalErrors, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(StatisticalErrors, self).modify_argument_parser(parser, args)

		self.StatisticalErrors_options = parser.add_argument_group("StatisticalErrors options")
		self.StatisticalErrors_options.add_argument("--relative-error", action="store_true", default=False,
				help="Do not plot absolute errors, but relative to bin content (in %).")

	def prepare_args(self, parser, plotData):
		super(StatisticalErrors, self).prepare_args(parser, plotData)

		# modify y label
		if plotData.plotdict["y_label"] == parser.get_default("y_label"):
			plotData.plotdict["y_label"] = "statistical error"
			if plotData.plotdict["relative_error"]:
				plotData.plotdict["y_label"] += " [%]"

	def run(self, plotData=None):
		super(StatisticalErrors, self).run(plotData)

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			# iterate over bins, replace contents with errors and divide by 
			# previous content, if 'relative_error' is True:
			for i in range(root_histogram.GetNbinsX()):
				new_bin_content = root_histogram.GetBinError(i)
				if plotData.plotdict["relative_error"]:
					content = root_histogram.GetBinContent(i)
					if content > 0:
						new_bin_content *= (100./content)
					else:
						new_bin_content = 0
				root_histogram.SetBinContent(i, new_bin_content)
				root_histogram.SetBinError(i, 0)

