# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import hashlib
import math

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.Utility.tools as tools

class PValue(analysisbase.AnalysisBase):
	"""Build PValue graphs from two histograms"""

	def __init__(self):
		super(PValue, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(PValue, self).modify_argument_parser(parser, args)
		
		self.pvalue_options = parser.add_argument_group("PValue options")
		self.pvalue_options.add_argument("--p-value-null-hypothesis-nicks", nargs="+", default=[None],
				help="Nick names for the null hypothesis.")		
		self.pvalue_options.add_argument("--p-value-alternative-hypothesis-nicks", nargs="+", default=[None],
				help="Nick names for the alternative hypothesis.")
		self.pvalue_options.add_argument("--p-value-observed-nicks", nargs="+", default=[None],
				help="Nick names for the observed q-value.")
		""" self.PValue_options.add_argument("--PValue-nicks", nargs="+", default=[None],
				help="Nick names for the resulting PValue graphs.")
		self.PValue_options.add_argument("--PValue-methods", nargs="+", default=["cp"],
				help="(Error) calculation methods. See TGraphAsymmErrors.Divide for further information. [Default: %(default)s]")
		"""
	def prepare_args(self, parser, plotData):
		super(PValue, self).prepare_args(parser, plotData)
		plotData.plotdict["p_value_null_hypothesis_nicks"] = [nick for nick in plotData.plotdict["p_value_null_hypothesis_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["p_value_alternative_hypothesis_nicks"] = [nick for nick in plotData.plotdict["p_value_alternative_hypothesis_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["p_value_observed_nicks"] = [nick for nick in plotData.plotdict["p_value_observed_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["p_value_null_hypothesis_nicks", "p_value_alternative_hypothesis_nicks","p_value_observed_nicks"])#, "PValue_nicks", "PValue_methods"])
		

		"""
		for index, nick in enumerate(plotData.plotdict["PValue_nicks"]):
			if not nick:
				nick = plotData.plotdict["PValue_numerator_nicks"][index] + "_" + plotData.plotdict["PValue_denominator_nicks"][index]
				plotData.plotdict["PValue_nicks"][index] = nick
			if not nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(plotData.plotdict["PValue_denominator_nicks"][index])+1, nick)
		"""
	def run(self, plotData=None):
		super(PValue, self).run(plotData)
		#print plotData.plotdict["p_value_null_hypothesis_nicks"]
		#print plotData.plotdict["p_value_alternative_hypothesis_nicks"]
		#print plotData.plotdict["p_value_observed_nicks"]
		#print plotData.plotdict["nicks"]
		#print plotData.plotdict["root_objects"]
		plotData.plotdict["root_objects"][plotData.plotdict["p_value_null_hypothesis_nicks"][0]].Print()
		
		for index, (p_value_null_hypothesis_nick, p_value_alternative_hypothesis_nick, p_value_observed_nick) in enumerate(zip(
				plotData.plotdict["p_value_null_hypothesis_nicks"],
				plotData.plotdict["p_value_alternative_hypothesis_nicks"],
				plotData.plotdict["p_value_observed_nicks"]
		)):
			print index, p_value_null_hypothesis_nick, p_value_alternative_hypothesis_nick
			null_hypothesis_histogramm=plotData.plotdict["root_objects"][p_value_null_hypothesis_nick]
			alternative_hypothesis_histogramm=plotData.plotdict["root_objects"][p_value_alternative_hypothesis_nick]
			q_obs=plotData.plotdict["root_objects"][p_value_observed_nick].GetX()[0]

			plotData.plotdict["root_objects"][p_value_observed_nick].SetPoint(plotData.plotdict["root_objects"][p_value_observed_nick].GetN(), q_obs,alternative_hypothesis_histogramm.GetMaximum())	
			#print q_obs		
	
			null_hypothesis_integral_full=null_hypothesis_histogramm.Integral()
			#print null_hypothesis_integral_full

			alternative_hypothesis_integral_full=alternative_hypothesis_histogramm.Integral()
			#print alternative_hypothesis_integral_full
			alternative_hypothesis_integral_fromq=alternative_hypothesis_histogramm.Integral(alternative_hypothesis_histogramm.FindBin(q_obs),alternative_hypothesis_histogramm.GetNbinsX()+1)

			null_hypothesis_integral_fromq=null_hypothesis_histogramm.Integral(0,null_hypothesis_histogramm.FindBin(q_obs))

			#print alternative_hypothesis_integral_fromq
			pvalue1=alternative_hypothesis_integral_fromq/alternative_hypothesis_integral_full
			print "pvalue=", pvalue1

			significance_in_sigma1 = tools.pvalue2sigma(pvalue1)

			print "sigmas1	= ", significance_in_sigma1

			pvalue2=null_hypothesis_integral_fromq/null_hypothesis_integral_full
			print "pvalue2=", pvalue2

			significance_in_sigma2 = tools.pvalue2sigma(pvalue2)

			print "sigmas2=", significance_in_sigma2

		"""
			PValue_graph_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_objects"][PValue_numerator_nick].GetName(),
			                                                             plotData.plotdict["root_objects"][PValue_denominator_nick].GetName()])).hexdigest()

			PValue_graph = ROOT.TGraphAsymmErrors(
					plotData.plotdict["root_objects"][PValue_numerator_nick],
					plotData.plotdict["root_objects"][PValue_denominator_nick]
			)
			PValue_graph.SetName(PValue_graph_name)
			PValue_graph.SetTitle("")
			plotData.plotdict["root_objects"][PValue_nick] = PValue_graph
		"""



