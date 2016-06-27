# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools


class SumOfHistograms(analysisbase.AnalysisBase):
	"""Create sum of histograms. This module does exactly the same as AddHistograms, but is can enable different addition steps together with this module."""
	
	def __init__(self):
		super(SumOfHistograms, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(SumOfHistograms, self).modify_argument_parser(parser, args)

		self.sum_histograms_options = parser.add_argument_group("{} options".format(self.name()))
		self.sum_histograms_options.add_argument(
				"--sum-nicks", nargs="+",
				help="Nick names (whitespace separated) for the histograms to be added"
		)
		self.sum_histograms_options.add_argument(
				"--sum-scale-factors", nargs="+",
				help="Scale factor (whitespace separated) for the histograms to be added [Default: 1]."
		)
		self.sum_histograms_options.add_argument(
				"--sum-result-nicks", nargs="+",
				help="Nick names for the resulting sum histograms."
		)

	def prepare_args(self, parser, plotData):
		super(SumOfHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["sum_nicks", "sum_result_nicks", "sum_scale_factors"])
		
		for index, (sum_nicks, sum_result_nick, sum_scale_factors) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["sum_nicks", "sum_result_nicks", "sum_scale_factors"]]
		)):
			plotData.plotdict["sum_nicks"][index] = sum_nicks.split()
			if sum_scale_factors is None:
				plotData.plotdict["sum_scale_factors"][index] = [1] * len(sum_nicks.split())
			else:
				plotData.plotdict["sum_scale_factors"][index] = [float(sum_scale_factor) for sum_scale_factor in sum_scale_factors.split()]
			if sum_result_nick is None:
				plotData.plotdict["sum_result_nicks"][index] = "sum_{}".format(
						"_".join(plotData.plotdict["sum_nicks"][index]),
				)
			if not plotData.plotdict["sum_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
						plotData.plotdict["nicks"].index(plotData.plotdict["sum_nicks"][index][0]),
						plotData.plotdict["sum_result_nicks"][index]
				)

	def run(self, plotData=None):
		super(SumOfHistograms, self).run(plotData)
		
		for sum_nicks, sum_scale_factors, sum_result_nick in zip(
				*[plotData.plotdict[k] for k in ["sum_nicks", "sum_scale_factors", "sum_result_nicks"]]
		):
			
			log.debug("SumOfHistograms: "+sum_result_nick+" = "+(" + ".join([str(scale)+"*"+nick for nick, scale in zip(sum_nicks, sum_scale_factors)])))
			plotData.plotdict["root_objects"][sum_result_nick] = roottools.RootTools.add_root_histograms(
					*[plotData.plotdict["root_objects"][nick] for nick in sum_nicks],
					scale_factors=sum_scale_factors
			)

