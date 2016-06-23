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
		self.sum_histograms_options.add_argument("--sum-nicks", nargs="+",
				help="Nick names for the histograms to sum up, whitespace separated.")
		self.sum_histograms_options.add_argument("--sum-scale-factors", nargs="+", default=["1.0"],
				help="Scale factors for the histograms to sum up, whitespace separated.")
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

	@staticmethod
	def return_sum_of_histograms(histograms, scale_factors=None, new_histogram_name=None):
		if(new_histogram_name == None):
			sum_nicks = []
			for histogram in histograms:
				sum_nicks.append( histogram.GetName())
			new_histogram_name = "histogram_" + hashlib.md5("_".join(sum_nicks)).hexdigest()
		if(scale_factors == None):
			scale_factors = [1] * len(histograms)

		new_histogram = None
		for histogram, scale_factor in zip(histograms, scale_factors):
			if isinstance(histogram, ROOT.TH1):
				if new_histogram == None:
					new_histogram = histogram.Clone(new_histogram_name)
					new_histogram.Scale(scale_factor)
				else:
					new_histogram.Add(histogram, scale_factor)
		return new_histogram

	def run(self, plotData=None):
		super(SumOfHistograms, self).run(plotData)
		
		for sum_nicks, sum_scale_factors, sum_result_nick in zip(plotData.plotdict["sum_nicks"],
		                                                         plotData.plotdict["sum_scale_factors"],
		                                                         plotData.plotdict["sum_result_nicks"]):
			
			log.debug("SumOfHistograms: "+sum_result_nick+" = "+(" + ".join([str(scale)+"*"+nick for nick, scale in zip(sum_nicks, sum_scale_factors)])))
			plotData.plotdict["root_objects"][sum_result_nick] = self.return_sum_of_histograms( [plotData.plotdict["root_objects"][nick] for nick in sum_nicks],
			                                                                               sum_scale_factors)
			
			# insert new nick between the existing ones
			if sum_result_nick not in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(max([plotData.plotdict["nicks"].index(nick) for nick in sum_nicks]),
				                                  sum_result_nick)

