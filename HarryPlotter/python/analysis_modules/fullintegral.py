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

class FullIntegral(analysisbase.AnalysisBase):
	def __init__(self):
		super(FullIntegral, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(FullIntegral, self).modify_argument_parser(parser, args)

		self.full_integral = parser.add_argument_group("Options for adding up histograms")
		self.full_integral.add_argument("--full-integral-nicks", nargs="+",
				help="Nick names of the histograms to sum up, whitespace separated")
		self.full_integral.add_argument("--full-integral-outputs", default="IntegralValues.txt",
				help="outputfile to save calculated integral value, None is no output written .")
	def prepare_args(self, parser, plotData):
		super(FullIntegral, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["full_integral_nicks", "full_integral_outputs"])

	def run(self, plotData=None):
		super(FullIntegral, self).run(plotData)
		outpath = os.path.expandvars(plotData.plotdict["full_integral_outputs"][0])
		#log.info(outpath)
		category = plotData.plotdict.get("category", "NoCategory")
		for integral_nicks in plotData.plotdict["full_integral_nicks"]:
			signal = SumOfHistograms.return_sum_of_histograms([plotData.plotdict["root_objects"][nick] for nick in integral_nicks.split(" ")])
			integral = 0
			for x_bin in xrange(1, signal.GetNbinsX()+1):
				#log.info("access bin %i"%x_bin)
				integral += signal.GetBinContent(x_bin)

			dirpath, filename = os.path.split(outpath)
			if not os.path.isdir(dirpath):
				os.makedirs(dirpath)
			if not os.path.exists(outpath):
				with open(outpath, "w") as outfile:
					outfile.write("%s: %f\n"%(category, integral))
			else:
				with open(outpath, "a") as outfile:
					outfile.write("%s: %f\n"%(category, integral))

