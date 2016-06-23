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
				"--sum-scale-factors", nargs="+",
				help="Scale factor (whitespace separated) for the histograms to be added [Default: 1]."
		)
		self.add_histograms_options.add_argument(
				"--sum-result-nicks", nargs="+",
				help="Nick names for the resulting sum histograms."
		)

	def prepare_args(self, parser, plotData):
		super(AddHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["histogram_nicks", "sum_result_nicks","sum_scale_factors"])
		
		for index, (histogram_nicks, sum_result_nick,sum_scale_factors) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["histogram_nicks", "sum_result_nicks","sum_scale_factors"]]
		)):
			plotData.plotdict["histogram_nicks"][index] = histogram_nicks.split()
			if sum_scale_factors is None:
				plotData.plotdict["sum_scale_factors"][index] = [1] * len(histogram_nicks.split())
			else:
				plotData.plotdict["sum_scale_factors"][index] = [float(sum_scale_factor) for sum_scale_factor in sum_scale_factors.split()]
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
		
		for histogram_nicks, sum_result_nick,sum_scale_factors in zip(
				*[plotData.plotdict[k] for k in ["histogram_nicks", "sum_result_nicks","sum_scale_factors"]]
		):
			sum_histogram = None
			log.debug("AddHistograms: "+sum_result_nick+" = "+(" + ".join([str(scale)+"*"+nick for nick, scale in zip(histogram_nicks, sum_scale_factors)])))
			for nick, sum_scale_factor in zip(histogram_nicks, sum_scale_factors):
				root_object = plotData.plotdict["root_objects"][nick]
				assert(isinstance(root_object, ROOT.TH1))
				if sum_histogram is None:
					sum_histogram = root_object.Clone(
							"sum_" + hashlib.md5("_".join([str(histogram_nicks), sum_result_nick])).hexdigest()
					)
					sum_histogram.Scale(sum_scale_factor)
					sum_histogram.SetDirectory(0)
				else:
					sum_histogram.Add(root_object, sum_scale_factor)
					sum_histogram.SetDirectory(0)

			plotData.plotdict["root_objects"][sum_result_nick] = sum_histogram

