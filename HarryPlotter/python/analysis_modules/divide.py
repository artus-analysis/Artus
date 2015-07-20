# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy
import hashlib

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools


class Divide(analysisbase.AnalysisBase):
	"""Divide selected histogram(s) (numerator) by other histogram(s) (denominator).
		Histograms are not divided in-place, but copies are created (result-nicks)."""

	def modify_argument_parser(self, parser, args):
		super(Divide, self).modify_argument_parser(parser, args)

		self.divide_options = parser.add_argument_group("{} options".format(self.name()))
		self.divide_options.add_argument("--divide-numerator-nicks", type=str, nargs="+",
				help="Nick names for the numerators of the divide.")
		self.divide_options.add_argument("--divide-denominator-nicks", type=str, nargs="+",
				help="Nick names for the denominator of the divide.")
		self.divide_options.add_argument("--divide-result-nicks", type=str, nargs="+",
				help="Nick names for the resulting divide graphs.")

	def prepare_args(self, parser, plotData):
		super(Divide, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["divide_numerator_nicks", "divide_denominator_nicks", "divide_result_nicks"])
		self.auto_set_arguments(plotData, ["divide_numerator_nicks", "divide_denominator_nicks"],
				"divide_result_nicks", "divide")

	def run(self, plotData=None):
		super(Divide, self).run(plotData)

		for numerator, denominator, new in zip(
			plotData.plotdict["divide_numerator_nicks"],
			plotData.plotdict["divide_denominator_nicks"],
			plotData.plotdict["divide_result_nicks"]
		):
			log.debug("Dividing %s by %s (new nick: %s)" % (numerator, denominator, new))
			new_name = "histogram_" + hashlib.md5("_".join([numerator, denominator, new])).hexdigest()
			new_histo = roottools.RootTools.to_histogram(plotData.plotdict["root_objects"][numerator]).Clone(new_name)
			new_histo.Divide(roottools.RootTools.to_histogram(plotData.plotdict["root_objects"][denominator]))
			plotData.plotdict["root_objects"][new] = new_histo
			plotData.plotdict["nicks"].append(new)
