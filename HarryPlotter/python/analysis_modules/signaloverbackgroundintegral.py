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

class SignalOverBackgroundIntegral(analysisbase.AnalysisBase):
	def __init__(self):
		super(SignalOverBackgroundIntegral, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(SignalOverBackgroundIntegral, self).modify_argument_parser(parser, args)

		self.sig_over_bkg_integral = parser.add_argument_group("Options for adding up histograms")
		self.sig_over_bkg_integral.add_argument("--sob-integral-background-nicks", nargs="+",
				help="Nick names of the background histograms to sum up, whitespace separated")
		self.sig_over_bkg_integral.add_argument("--sob-integral-signal-nicks", nargs="+",
				help="Nick names of the signal histograms to sum up, whitespace separated")
		self.sig_over_bkg_integral.add_argument("--sob-integral-result-nicks", nargs="+", default=["sob_integral_ratio"],
				help="Nick names for the resulting histograms.")
		self.sig_over_bkg_integral.add_argument("--sob-integral-method", nargs="+", default=["soversqrtb"], choices=["ams", "soversqrtb", "soverb"],
				help="Desired method to calculate sob_integral condition.")

	def prepare_args(self, parser, plotData):
		super(SignalOverBackgroundIntegral, self).prepare_args(parser, plotData)


		self.prepare_list_args(plotData, ["sob_integral_background_nicks", "sob_integral_signal_nicks", "sob_integral_result_nicks", "sob_integral_method"])

	def run(self, plotData=None):
		super(SignalOverBackgroundIntegral, self).run(plotData)
		max_y_bin = 0
		y_max = -1.0
		found_trans = False
		trans_y_bin = 0
		trans_y_value = 0
		for signal_nick, background_nick, result_nick, method in  zip(
																	plotData.plotdict["sob_integral_signal_nicks"],
																	plotData.plotdict["sob_integral_background_nicks"],
																	plotData.plotdict["sob_integral_result_nicks"],
																	plotData.plotdict["sob_integral_method"]):

			signal = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in signal_nick.split(" ")])
			#new_histogram = signal.Clone("storage_histogram")
			background = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in background_nick.split(" ")])

			if(signal.GetNbinsX() != background.GetNbinsX()):
				log.fatal("Signal histogram " + signal + " has a different binning than " + background )

			#for x_bin in xrange(1, signal.GetNbinsX()+1):
				#new_histogram.SetBinContent(x_bin, expression( signal.GetBinContent(x_bin), background.GetBinContent(x_bin), regularization_parameter))

			output_histogram = signal.Clone(result_nick)
			sig_events = 0.0
			bkg_events = 0.0
			bin_max = signal.GetNbinsX()
			if method == "righttoleft":
				for x_bin in xrange(0, bin_max):
					sig_events += signal.GetBinContent(bin_max-x_bin)
					bkg_events += background.GetBinContent(bin_max-x_bin)
					if bkg_events > 0:
						value = sig_events/bkg_events**0.5
						output_histogram.SetBinContent(bin_max-x_bin, value)
						if value > y_max:
							max_y_bin = bin_max-x_bin
							y_max = value
					else:
						output_histogram.SetBinContent(bin_max-x_bin, 0)

			elif method == "lefttoright":
				found_trans = False
				trans_y_bin = 0
				trans_y_value = 0
				for x_bin in xrange(1, bin_max+1):
					sig_events += signal.GetBinContent(x_bin)
					bkg_events += background.GetBinContent(x_bin)
					if bkg_events > 0:
						output_histogram.SetBinContent(x_bin, sig_events/bkg_events**0.5)
						if not found_trans and sig_events/bkg_events**0.5 > 0.15:
							found_trans = True
							trans_y_bin = x_bin
							trans_y_value = sig_events/bkg_events**0.5
					else:
						output_histogram.SetBinContent(x_bin, 0)

			elif method == "combination":
				for x_bin in range(trans_y_bin, max_y_bin):
					sig_events += signal.GetBinContent(x_bin)
					bkg_events += background.GetBinContent(x_bin)
				value = sig_events/bkg_events**0.5
				for x_bin in range(1, bin_max+1):
					if x_bin < trans_y_bin:
						output_histogram.SetBinContent(x_bin, trans_y_value)
					elif trans_y_bin <= x_bin and x_bin < max_y_bin:
						output_histogram.SetBinContent(x_bin, value)
					elif max_y_bin <= x_bin:
						output_histogram.SetBinContent(x_bin, y_max)
					else:
						output_histogram.SetBinContent(x_bin, 0)

			plotData.plotdict["root_objects"][result_nick] = output_histogram
			plotData.plotdict["nicks"].append(result_nick)