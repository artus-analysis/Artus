# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class AddHistograms(analysisbase.AnalysisBase):
	"""Add sum of histograms"""

	def modify_argument_parser(self, parser, args):
		super(AddHistograms, self).modify_argument_parser(parser, args)

		self.add_histograms_options = parser.add_argument_group("{} options".format(self.name()))
		self.add_histograms_options.add_argument(
				"--histogram-nicks", nargs="+",
				help="Nick names (whitespace separated) for the histograms to be added"
		)
		self.add_histograms_options.add_argument(
				"--sum-result-nicks", nargs="+",
				help="Nick names for the resulting sum histograms."
		)

	def prepare_args(self, parser, plotData):
		super(AddHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["histogram_nicks", "sum_result_nicks"])
		
		for index, (histogram_nicks, sum_result_nick) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["histogram_nicks", "sum_result_nicks"]]
		)):
			plotData.plotdict["histogram_nicks"][index] = histogram_nicks.split()
			if sum_result_nick is None:
				plotData.plotdict["sum_result_nicks"][index] = "sum_{}".format(
						"_".join(plotData.plotdict["histogram_nicks"][index]),
				)
			if not plotData.plotdict["sum_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
						plotData.plotdict["nicks"].index(plotData.plotdict["histogram_nicks"][index][0]),
						plotData.plotdict["sum_result_nicks"][index]
				)

	def run(self, plotData=None):
		super(AddHistograms, self).run(plotData)
		
		for histogram_nicks, sum_result_nick in zip(
				*[plotData.plotdict[k] for k in ["histogram_nicks", "sum_result_nicks"]]
		):
			sum_histogram = None
			for nick in histogram_nicks:
				root_object = plotData.plotdict["root_objects"][nick]
				assert(isinstance(root_object, ROOT.TH1))
				if sum_histogram is None:
					sum_histogram = root_object.Clone(
							"sum_" + hashlib.md5("_".join([str(histogram_nicks), sum_result_nick])).hexdigest()
					)
					sum_histogram.SetDirectory(0)
				else:
					sum_histogram.Add(root_object)
					sum_histogram.SetDirectory(0)
			plotData.plotdict["root_objects"][sum_result_nick] = sum_histogram

