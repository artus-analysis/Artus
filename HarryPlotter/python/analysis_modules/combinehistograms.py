# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class CombineHistograms(analysisbase.AnalysisBase):
	"""Combine histograms. Non-empty bins replace the bins in the combined histogram iteratively."""

	def modify_argument_parser(self, parser, args):
		super(CombineHistograms, self).modify_argument_parser(parser, args)

		self.combine_histograms_options = parser.add_argument_group("{} options".format(self.name()))
		self.combine_histograms_options.add_argument(
				"--combine-histogram-nicks", nargs="+", default=[],
				help="Nick names (whitespace separated) for the histograms to be combined. [Default: all]"
		)
		self.combine_histograms_options.add_argument(
				"--combine-result-nicks", nargs="+",
				help="Nick names for the resulting sum histograms."
		)

	def prepare_args(self, parser, plotData):
		super(CombineHistograms, self).prepare_args(parser, plotData)
		
		if len(plotData.plotdict["combine_histogram_nicks"]) == 0:
			plotData.plotdict["combine_histogram_nicks"] = [" ".join(plotData.plotdict["nicks"])]
		
		self.prepare_list_args(plotData, ["combine_histogram_nicks", "combine_result_nicks"])
		
		for index, (histogram_nicks, result_nick) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["combine_histogram_nicks", "combine_result_nicks"]]
		)):
			plotData.plotdict["combine_histogram_nicks"][index] = histogram_nicks.split()
			if result_nick is None:
				plotData.plotdict["combine_result_nicks"][index] = "sum_{}".format(
						"_".join(plotData.plotdict["combine_histogram_nicks"][index]),
				)
			if not plotData.plotdict["combine_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
						plotData.plotdict["nicks"].index(plotData.plotdict["combine_histogram_nicks"][index][0]),
						plotData.plotdict["combine_result_nicks"][index]
				)

	def run(self, plotData=None):
		super(CombineHistograms, self).run(plotData)
		
		for index, (histogram_nicks, result_nick) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["combine_histogram_nicks", "combine_result_nicks"]]
		)):
			result_histogram = None
			for nick in histogram_nicks:
				root_histogram = plotData.plotdict["root_objects"][nick]
				assert(isinstance(root_histogram, ROOT.TH1))
				if result_histogram is None:
					result_histogram = root_histogram.Clone(
							"combine_" + hashlib.md5("_".join([str(histogram_nicks), result_nick])).hexdigest()
					)
					result_histogram.SetDirectory(0)
				else:
					for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
						for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
							for z_bin in xrange(1, root_histogram.GetNbinsZ()+1):
								global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
								if root_histogram.GetBinContent(global_bin) != 0.0:
									result_histogram.SetBinContent(global_bin, root_histogram.GetBinContent(global_bin))
									result_histogram.SetBinError(global_bin, root_histogram.GetBinError(global_bin))

			plotData.plotdict["root_objects"][result_nick] = result_histogram

