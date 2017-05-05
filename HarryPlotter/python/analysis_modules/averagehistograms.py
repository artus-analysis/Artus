# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT
import math
import numpy

import Artus.HarryPlotter.analysisbase as analysisbase
from Artus.HarryPlotter.analysis_modules.sumofhistograms import SumOfHistograms

class AverageHistograms(analysisbase.AnalysisBase):
	def __init__(self):
		super(AverageHistograms, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(AverageHistograms, self).modify_argument_parser(parser, args)
		
		self.average_options = parser.add_argument_group("Options for adding up histograms")
		self.average_options.add_argument("--to-average-nicks", nargs="+",
				help="Nick names of histograms to be averaged")
		self.average_options.add_argument("--average-result-nick", nargs="+",
				help="Nick name of the resulting average histogram")
		self.average_options.add_argument("--averaging-method", default="maxdiff",
				help="Combination method. Available: minmax, average, maxdiff, rms")
		
	@staticmethod
	def rms(ivec):
		result = 0
		for entry in ivec:
			result += entry * entry
		result /= len(ivec)
		result = math.sqrt(result)
		return result

	@staticmethod
	def mse(ivec):
		rms = AverageHistograms.rms(ivec)
		return AverageHistograms.rms([ (value - rms) for value in ivec])

	def run(self, plotData=None):
		super(AverageHistograms, self).run(plotData)
		# create output histogram
		values = []
		import pprint
		pprint.pprint(plotData.plotdict["root_objects"])
		result_histogram = plotData.plotdict["root_objects"][plotData.plotdict["to_average_nicks"][0]].Clone(plotData.plotdict["average_result_nick"])
		for n_bin in xrange(1, result_histogram.GetNbinsX()+1): 
			for histogram in plotData.plotdict["to_average_nicks"]:
				values.append(plotData.plotdict["root_objects"][histogram].GetBinContent(n_bin))
			if(plotData.plotdict["averaging_method"] == "minmax"):
				result_histogram.SetBinContent(n_bin, (max(values) + min(values)) / 2)
			elif(plotData.plotdict["averaging_method"] == "average"):
				result_histogram.SetBinContent(n_bin, sum(values) / len(values))
			elif(plotData.plotdict["averaging_method"] == "maxdiff"):
				result_histogram.SetBinContent(n_bin, (max(values) - min(values)) / 2)
			elif(plotData.plotdict["averaging_method"] == "rms"):
				result_histogram.SetBinContent(n_bin, self.mse(values))
			elif(plotData.plotdict["averaging_method"] == "rmssum"):
				result_histogram.SetBinContent(n_bin, (sum(numpy.array(values)**2)**0.5))
			values = []
		plotData.plotdict["root_objects"][plotData.plotdict["average_result_nick"]] = result_histogram
		plotData.plotdict["nicks"].append(plotData.plotdict["average_result_nick"])
