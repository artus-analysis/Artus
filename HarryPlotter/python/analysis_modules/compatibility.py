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
import Artus.HarryPlotter.utility.roottools as roottools




class Compatibility(analysisbase.AnalysisBase):
	"""Compare the compatibility of two distributions with the Kolmogorov-Smirnov-Test"""

	def __init__(self):
		super(Compatibility, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Compatibility, self).modify_argument_parser(parser, args)
		
		self.compatibility_options = parser.add_argument_group("Compatibility options")
		self.compatibility_options.add_argument(
				"--compatibility-base", nargs="+", default=[None],
				help="Nick names for the histograms to compare to."
		)
		self.compatibility_options.add_argument(
				"--compatibility-to-compare", nargs="+", default=[None],
				help="Nick names for histograms that should be probed."
		)
		self.compatibility_options.add_argument(
				"--compatibility-result-nick", nargs="+", default=[None],
				help="Histogram to save the result to."
		)

	def run(self, plotData=None):
		super(Compatibility, self).run(plotData)
		
		for index, (base, compare_histograms, new_histogram_name) in enumerate(zip(
				plotData.plotdict["compatibility_base"],
				plotData.plotdict["compatibility_to_compare"],
				plotData.plotdict["compatibility_result_nick"],
		)):
			new_binning = str(len(compare_histograms.split(" "))) + ",-0.5," + str(len(compare_histograms.split(" "))+0.5)
			print new_binning
			new_histogram = roottools.RootTools.create_root_histogram([new_binning], name = new_histogram_name)
			plotData.plotdict["nicks"].append(new_histogram_name)
			for index, histo in enumerate(compare_histograms.split(" ")):
				print index
				new_histogram.SetBinContent(index-1, plotData.plotdict["root_objects"][base].KolmogorovTest(plotData.plotdict["root_objects"][histo], "D"))
			plotData.plotdict["root_objects"][new_histogram_name] = new_histogram

