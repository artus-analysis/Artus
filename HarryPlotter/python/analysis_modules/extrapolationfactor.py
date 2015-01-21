# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class ExtrapolationFactor(analysisbase.AnalysisBase):
	def __init__(self):
		super(ExtrapolationFactor, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(ExtrapolationFactor, self).modify_argument_parser(parser, args)
		
		self.extrapolation_factor_options = parser.add_argument_group("Extrapolation factor options")
		self.extrapolation_factor_options.add_argument("--extrapolation-numerator-nicks", nargs="+", required=True,
				help="Nick names for the numerators of the extrapolation factors.")
		self.extrapolation_factor_options.add_argument("--extrapolation-denominator-nicks", nargs="+", required=True,
				help="Nick names for the denominator of the extrapolation factors.")
		self.extrapolation_factor_options.add_argument("--extrapolation-nicks", nargs="+", default=[None],
				help="Nick names for the histograms to scale the extrapolation factors.")

	def prepare_args(self, parser, plotData):
		super(ExtrapolationFactor, self).prepare_args(parser, plotData)
		
		plotData.plotdict["extrapolation_numerator_nicks"] = [nick for nick in plotData.plotdict["extrapolation_numerator_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["extrapolation_denominator_nicks"] = [nick for nick in plotData.plotdict["extrapolation_denominator_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["extrapolation_nicks"] = [nick for nick in plotData.plotdict["extrapolation_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["extrapolation_numerator_nicks", "extrapolation_denominator_nicks", "extrapolation_nicks"])
		
		for index, nick in enumerate(plotData.plotdict["extrapolation_nicks"]):
			if not nick:
				nick = plotData.plotdict["extrapolation_numerator_nicks"][index] + "_" + plotData.plotdict["extrapolation_denominator_nicks"][index]
				plotData.plotdict["extrapolation_nicks"][index] = nick
		
	
	def run(self, plotData=None):
		super(ExtrapolationFactor, self).run(plotData)
		
		for index, (extrapolation_numerator_nick, extrapolation_denominator_nick, extrapolation_nick) in enumerate(zip(plotData.plotdict["extrapolation_numerator_nicks"],
		                                                                       plotData.plotdict["extrapolation_denominator_nicks"],
		                                                                       plotData.plotdict["extrapolation_nicks"])):
			
			new_histogram_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_objects"][extrapolation_numerator_nick].GetName(),
			                                                          plotData.plotdict["root_objects"][extrapolation_denominator_nick].GetName()])).hexdigest()
			
			extrapolation_numerator = plotData.plotdict["root_objects"][extrapolation_numerator_nick].Integral()
			extrapolation_denominator = plotData.plotdict["root_objects"][extrapolation_denominator_nick].Integral()
			extrapolation_factor = 1.0
			if extrapolation_denominator == 0.0:
				log.error("Extrapolation denominator is empty, which results in a division by zero! The extrapolation factor is set to 1.0.")
			else:
				extrapolation_factor = extrapolation_numerator / extrapolation_denominator
			
			plotData.plotdict["root_objects"][extrapolation_nick].Scale(extrapolation_factor)

