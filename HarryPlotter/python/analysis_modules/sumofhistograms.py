# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class SumOfHistograms(analysisbase.AnalysisBase):
	def __init__(self):
		super(SumOfHistograms, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(SumOfHistograms, self).modify_argument_parser(parser, args)
		
		self.sum_histograms_options = parser.add_argument_group("Options for adding up histograms")
		self.sum_histograms_options.add_argument("--sum-nicks", nargs="+", required=True,
				help="Nick names for the histograms to sum up, whitespace separated.")
		self.sum_histograms_options.add_argument("--sum-scale-factors, whitespace separated.", nargs="+", default=["1.0"],
				help="Scale factors for the histograms to sum up.")
		self.sum_histograms_options.add_argument("--sum-result-nicks", nargs="+", default=[None],
				help="Nick names for the resulting histograms.")

	def prepare_args(self, parser, plotData):
		super(SumOfHistograms, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["sum_nicks", "sum_scale_factors", "sum_result_nicks"])
		
		plotData.plotdict["sum_nicks"] = [[nick for nick in nicks.split() if nick in plotData.plotdict["root_objects"]] for nicks in plotData.plotdict["sum_nicks"]]
		
		plotData.plotdict["sum_scale_factors"] = [[float(factor) for factor in factors.split()] for factors in plotData.plotdict["sum_scale_factors"]]
		for index, (nicks, factors) in enumerate(zip(plotData.plotdict["sum_nicks"],
		                                             plotData.plotdict["sum_scale_factors"])):
			plotData.plotdict["sum_scale_factors"][index] = (factors * len(nicks))[:len(nicks)]
		
		for index, nick in enumerate(plotData.plotdict["sum_result_nicks"]):
			if not nick:
				plotData.plotdict["sum_result_nicks"][index] = "_".join(plotData.plotdict["sum_nicks"][index])
	
	def run(self, plotData=None):
		super(SumOfHistograms, self).run(plotData)
		
		for sum_nicks, sum_scale_factors, sum_result_nick in zip(plotData.plotdict["sum_nicks"],
		                                                         plotData.plotdict["sum_scale_factors"],
		                                                         plotData.plotdict["sum_result_nicks"]):
			
			new_histogram_name = "histogram_" + hashlib.md5("_".join(sum_nicks)).hexdigest()
			new_histogram = None
			
			for histogram, scale_factor in zip([plotData.plotdict["root_objects"][nick] for nick in sum_nicks], sum_scale_factors):
				if isinstance(histogram, ROOT.TH1):
					if new_histogram == None:
						new_histogram = histogram.Clone(new_histogram_name)
						new_histogram.Scale(scale_factor)
					else:
						new_histogram.Add(histogram, scale_factor)
			
			plotData.plotdict["root_objects"][sum_result_nick] = new_histogram
			
			# insert new nick between the existing ones
			if sum_result_nick not in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(max([plotData.plotdict["nicks"].index(nick) for nick in sum_nicks]),
				                                  sum_result_nick)

