# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT
import math

import Artus.HarryPlotter.analysisbase as analysisbase
from Artus.HarryPlotter.analysis_modules.sumofhistograms import SumOfHistograms

class BlindingPolicy(analysisbase.AnalysisBase):
	def __init__(self):
		super(BlindingPolicy, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(BlindingPolicy, self).modify_argument_parser(parser, args)
		
		self.blinding_policy_options = parser.add_argument_group("Options for adding up histograms")
		self.blinding_policy_options.add_argument("--blinding-signal-nicks", nargs="+",
				help="Nick names of the signal histograms to sum up, whitespace separated")
		self.blinding_policy_options.add_argument("--blinding-background-nicks", nargs="+",
				help="Nick names of the background histograms to sum up, whitespace separated")
		self.blinding_policy_options.add_argument("--blinding-result-nicks", nargs="+", default=["blinding_ratio"],
				help="Nick names for the resulting histograms.")
		self.blinding_policy_options.add_argument("--blinding-method", nargs="+", default=["soversqrtb"], choices=["ams", "soversqrtb", "soverb"],
				help="Desired method to calculate blinding condition.")
		self.blinding_policy_options.add_argument("--blinding-parameters", nargs="+", default=[0.0], type=float,
				help="Regularization parameter for AMS method or epsilon for soversqrtb an epsilon to account for systematic uncertainties")

	def prepare_args(self, parser, plotData):
		super(BlindingPolicy, self).prepare_args(parser, plotData)

		
		self.prepare_list_args(plotData, ["blinding_signal_nicks", "blinding_background_nicks", "blinding_result_nicks", "blinding_method", "blinding_parameters"])
		
	@staticmethod
	def get_blinding_expression(name):
		if (name == "ams"): 
			return lambda signal, background, parameter:  pow(2 * ((signal + background + parameter) * math.log1p( signal/(background+parameter)) - signal), 0.5) if((background + parameter) >0 ) else 0
		elif (name == "soversqrtb"):
			return lambda signal, background, parameter:  (signal / pow(background + pow(parameter*background, 2), 0.5)) if (background > 0) else 0.0
		elif (name == "soverb"):
			return lambda signal, background, parameter: signal / background
		else:
			log.fatal("invalid blinding method string selected: " + name)
	
	def run(self, plotData=None):
		super(BlindingPolicy, self).run(plotData)

		for signal_nick, background_nick, result_nick, method, regularization_parameter in  zip(
		                                                         plotData.plotdict["blinding_signal_nicks"],
		                                                         plotData.plotdict["blinding_background_nicks"],
		                                                         plotData.plotdict["blinding_result_nicks"],
		                                                         plotData.plotdict["blinding_method"],
		                                                         plotData.plotdict["blinding_parameters"] ):
			
			signal = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in signal_nick.split(" ")])
			new_histogram = signal.Clone(result_nick)
			background = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in background_nick.split(" ")])
			expression = self.get_blinding_expression(method)

			if(signal.GetNbinsX() != background.GetNbinsX()):
				log.fatal("Signal histogram " + signal + " has a different binning than " + background )

			for x_bin in xrange(1, signal.GetNbinsX()+1):
				new_histogram.SetBinContent(x_bin, expression( signal.GetBinContent(x_bin), background.GetBinContent(x_bin), regularization_parameter) )
				log.debug("bin: {bin}\tsig: {sig}\tbkg:{bkg}".format(bin=x_bin, sig=signal.GetBinContent(x_bin), bkg=background.GetBinContent(x_bin)))

			plotData.plotdict["root_objects"][result_nick] = new_histogram
			plotData.plotdict["nicks"].append(result_nick)

