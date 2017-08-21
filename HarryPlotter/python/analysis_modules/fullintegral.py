# -*- coding: utf-8 -*-
"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import os
import sys
import hashlib
import ROOT
import math
import Artus.Utility.jsonTools as jsonTools
import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools

class FullIntegral(analysisbase.AnalysisBase):
	
	def __init__(self):
		super(FullIntegral, self).__init__()
		
	def remove_options(self, parser, options):
		for option in options:
			for action in parser._actions:
				if vars(action)['option_strings'][0] == option:
					parser._handle_conflict_resolve(None, [(option, action)])
					break
	
	def modify_argument_parser(self, parser, args):
		super(FullIntegral, self).modify_argument_parser(parser, args)

		self.full_integral = parser.add_argument_group("Options for adding up histograms")
		self.full_integral.add_argument("--full-integral-nicks", nargs="+",
				help="Nick names of the histograms to sum up, whitespace separated")
		self.full_integral.add_argument("--full-integral-outputs", default="./IntegralValues.txt",
				help="outputfile to save calculated integral value, None is no output written .")
		self.full_integral.add_argument("--full-integral-task", default="standard", choices=["standard", "int_to_histogram"],
				help="standard: add nicks, integrate sum, write to file -- int_to_histogram: integrate each nick, remove them from plotlist, fill into new histogram, add to plotlist")
		self.full_integral.add_argument("--full-integral-binning", default="25,0,100",
				help="binning is only used if one choses task=int_to_histogram: [Defaul:%(default)s]NBins,XMin,XMax")

	def prepare_args(self, parser, plotData):
		super(FullIntegral, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["full_integral_nicks", "full_integral_outputs"])
		if plotData.plotdict["full_integral_task"] == "int_to_histogram":
			self.remove_options(parser, ["--x-bins", "--labels", "--colors", "--scale_factors", "--legend-markers", "--nicks"])

	def run(self, plotData=None):
		super(FullIntegral, self).run(plotData)
		outpath = os.path.expandvars(plotData.plotdict["full_integral_outputs"][0])

		category = plotData.plotdict.get("category", "NoCategory")
		if plotData.plotdict.get("full_integral_task", "standard") == "standard":
			for integral_nicks in plotData.plotdict["full_integral_nicks"]:
				goodnicks = []

				for nick in integral_nicks.split(" "):
					try:
						roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick]])
						goodnicks.append(nick)
					except:
						log.error("Can't add " + nick + " to the full integral")

				signal = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick] for nick in goodnicks])
				integral = 0
				for x_bin in xrange(1, signal.GetNbinsX() + 1):
					integral += signal.GetBinContent(x_bin)

				dirpath, filename = os.path.split(outpath)
				if not os.path.isdir(dirpath): os.makedirs(dirpath)

				with open(outpath, "a" if os.path.exists(outpath) else "w") as outfile:
					outfile.write("nick: %s category: %s integral: %f\n"%(nick, category, integral))
		else:
			integral_values = []
			integral_nicks = []
			for nick, histogram in plotData.plotdict.get("root_objects", {}).iteritems():
				integral = 0
				for x_bin in xrange(1, histogram.GetNbinsX() + 1):
					integral += histogram.GetBinContent(x_bin)
				integral_values.append(integral)
				integral_nicks.append(nick)

				# Delete and reassign to get rid of those nasty root objects without disturbing pythons iterator
				del plotData.plotdict["root_objects"][nick]
				plotData.plotdict["root_objects"][nick] = None

			histogram = ROOT.TH1F("FullIntegralHistogram", "FullIntegralHistogram", *map(int,plotData.plotdict.get("full_integral_binning", "25,0,100").split(",")))

			dirpath, filename = os.path.split(outpath)
			if not os.path.isdir(dirpath):
				os.makedirs(dirpath)

			with open(outpath, "w") as outfile:
				for nick, integral in zip(integral_nicks, integral_values):
					outfile.write("%s: %f\n"%(nick, integral))
					histogram.Fill(integral)

			plotData.plotdict["root_objects"] = {"FullIntegral" : histogram}
			plotData.plotdict["nicks"] = ["FullIntegral"]
			plotData.plotdict["x_bins"] = [plotData.plotdict["full_integral_binning"]]
			plotData.plotdict["scale_factors"] = [1.0]
			plotData.plotdict["markers"] = [plotData.plotdict["markers"][0]]
			plotData.plotdict["colors"] = [plotData.plotdict["colors"][0]]
			plotData.plotdict["labels"] = [plotData.plotdict["labels"][0]]
			plotData.plotdict["legend_markers"] = [plotData.plotdict["legend_markers"][0]]
			plotData.plotdict["files"] = [plotData.plotdict["files"][0]]
			plotData.plotdict["folders"] = [plotData.plotdict["folders"][0]]
			plotData.plotdict["weights"] = [plotData.plotdict["weights"][0]]
			plotData.plotdict["stacks"] = [plotData.plotdict["stacks"][0]]

			del plotData.plotdict["root_trees"]
			plotData.input_json_dicts = []
			log.info(plotData.plotdict)
