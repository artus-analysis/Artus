# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import os
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
		self.sig_over_bkg_integral.add_argument("--sob-integral-direction", nargs="+", default=["righttoleft"], choices=["righttoleft", "rcombination"],
				help="Desired direction to calculate sob_integral condition.")
		self.sig_over_bkg_integral.add_argument("--sob-integral-method", nargs="+", default=["soversqrtb"], choices=["soversqrtb", "soversplusb"],
				help="Desired direction to calculate sob_integral condition.")
		self.sig_over_bkg_integral.add_argument("--sob-integral-outputs", nargs="+", default=[None],
				help="Desired outputfile to save calculated min/max, None is no output written .")
		self.sig_over_bkg_integral.add_argument("--sob-frontname", default = "",
				help="this string is written in front of the output string [Default: %(default)s]")
	def prepare_args(self, parser, plotData):
		super(SignalOverBackgroundIntegral, self).prepare_args(parser, plotData)


		self.prepare_list_args(plotData, ["sob_integral_background_nicks", "sob_integral_signal_nicks", "sob_integral_result_nicks", "sob_integral_direction","sob_integral_direction", "sob_integral_outputs"])
	def get_function(self, name):
		if name=="soversqrtb":
			return lambda s,b : s/(b)**0.5
		elif name=="soversqrtsplusb":
			return lambda s,b : s/(b+s)**0.5
		elif name=="soversplusb":
			return lambda s,b : s/(b+s)**0.5
		else:
			log.fatal("invalid integration method string selected: " + name)
	def run(self, plotData=None):
		super(SignalOverBackgroundIntegral, self).run(plotData)
		rtl_marks = []
		rtl_yvalues = []
		ltr_marks = []
		ltr_yvalues = []
		front_name = plotData.plotdict["sob_frontname"]
		for signal_nick, background_nick, result_nick, direction, method, outpath in  zip(
																	plotData.plotdict["sob_integral_signal_nicks"],
																	plotData.plotdict["sob_integral_background_nicks"],
																	plotData.plotdict["sob_integral_result_nicks"],
																	plotData.plotdict["sob_integral_direction"],
																	plotData.plotdict["sob_integral_method"],
																	plotData.plotdict["sob_integral_outputs"]):

			signal = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in signal_nick.split(" ")])
			#new_histogram = signal.Clone("storage_histogram")
			background = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in background_nick.split(" ")])
			calculation_function = self.get_function(method)
			if(signal.GetNbinsX() != background.GetNbinsX()):
				log.fatal("Signal histogram " + signal + " has a different binning than " + background )
			#for x_bin in xrange(1, signal.GetNbinsX()+1):
				#new_histogram.SetBinContent(x_bin, expression( signal.GetBinContent(x_bin), background.GetBinContent(x_bin), regularization_parameter))
			if len(rtl_marks)+len(ltr_marks) == 0:
				rtl_marks.append(signal.GetNbinsX()+1)
				bin_max = signal.GetNbinsX()
				ltr_marks.append(1)
				bin_min = 1
			elif len(rtl_marks) == 0:
				rtl_marks.append(signal.GetNbinsX()+1)
				bin_max = signal.GetNbinsX()
			elif len(ltr_marks) == 0:
				ltr_marks.append(1)
				bin_min = 1
			else:
				bin_max = signal.GetNbinsX()
				bin_min = 1

			output_histogram = signal.Clone(result_nick)
			sig_events = 0.0
			bkg_events = 0.0

			if direction == "righttoleft":
				y_values = [0,]
				x_values = [1,]
				#iterate over full range histogram
				for x_bin in xrange(bin_max, bin_min-1,-1):
					#reset calculation at every maximum mark and restart calculating
					for mark in rtl_marks:
						if x_bin == mark-1:
							sig_events = 0.0
							bkg_events = 0.0
					sig_events += signal.GetBinContent(x_bin)
					bkg_events += background.GetBinContent(x_bin)
					value = 0
					if bkg_events > 1 and sig_events > 1:
						value = calculation_function(sig_events, bkg_events)
						output_histogram.SetBinContent(x_bin, value)
					else:
						output_histogram.SetBinContent(x_bin, 0)
					if x_bin < rtl_marks[-1]:
						y_values.append(value)
						x_values.append(x_bin)
				y_max = max(y_values)
				x_bin = x_values[y_values.index(y_max)]
				rtl_marks.append(x_bin)
				rtl_yvalues.append(y_max)
			elif direction == "rcombination":
				rtl_marks.pop(-1)
				rtl_yvalues.pop(-1)
				sig_evt = 0
				bkg_evt = 0
				for x_bin in range(1, rtl_marks[-1]):
					sig_events += signal.GetBinContent(x_bin)
					bkg_events += background.GetBinContent(x_bin)
				if bkg_events > 1 and sig_events > 1:
					rtl_yvalues.append(calculation_function(sig_events, bkg_events))
				else:
					rtl_yvalues.append(0)
				rtl_marks.append(1)
				rtl_yvalues.reverse()
				rtl_marks.reverse()
				for x_bin in range(1, signal.GetNbinsX()+1,1):
					for left, right, value in zip(rtl_marks[:-1], rtl_marks[1:], rtl_yvalues):
						if left <= x_bin and x_bin < right:
							output_histogram.SetBinContent(x_bin, value)
				dirpath, filename = os.path.split(outpath)
				if not os.path.isdir(dirpath):
					os.makedirs(dirpath)
				if not os.path.exists(outpath):
					with open(outpath, "w") as outfile:
						outfile.write(front_name+"%s : %s\n"%(plotData.plotdict["x_expressions"][0], " ".join(["%s"%(signal.GetBinLowEdge(x)) for x in rtl_marks])))
				else:
					with open(outpath, "a") as outfile:
						outfile.write(front_name+"%s : %s\n"%(plotData.plotdict["x_expressions"][0], " ".join(["%s"%(signal.GetBinLowEdge(x)) for x in rtl_marks])))
			plotData.plotdict["root_objects"][result_nick] = output_histogram
			plotData.plotdict["nicks"].append(result_nick)