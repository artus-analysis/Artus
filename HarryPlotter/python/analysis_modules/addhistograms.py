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
				"--add-nicks", nargs="+",
				help="Nick names (whitespace separated) for the histograms to be added"
		)
		self.add_histograms_options.add_argument(
				"--add-scale-factors", nargs="+",
				help="Scale factor (whitespace separated) for the histograms to be added [Default: 1]."
		)
		self.add_histograms_options.add_argument(
				"--add-result-nicks", nargs="+",
				help="Nick names for the resulting sum histograms."
		)

	def prepare_args(self, parser, plotData):
		super(AddHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["add_nicks", "add_result_nicks", "add_scale_factors"])
		
		for index, (add_nicks, add_result_nick, add_scale_factors) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["add_nicks", "add_result_nicks", "add_scale_factors"]]
		)):
			plotData.plotdict["add_nicks"][index] = add_nicks.split()
			if add_scale_factors is None:
				plotData.plotdict["add_scale_factors"][index] = [1] * len(add_nicks.split())
			else:
				plotData.plotdict["add_scale_factors"][index] = [float(add_scale_factor) for add_scale_factor in add_scale_factors.split()]
			if add_result_nick is None:
				plotData.plotdict["add_result_nicks"][index] = "add_{}".format(
						"_".join(plotData.plotdict["add_nicks"][index]),
				)
			if not plotData.plotdict["add_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
						plotData.plotdict["nicks"].index(plotData.plotdict["add_nicks"][index][0]),
						plotData.plotdict["add_result_nicks"][index]
				)

	def run(self, plotData=None):
		super(AddHistograms, self).run(plotData)
		
		for add_nicks, add_result_nick, add_scale_factors in zip(
				*[plotData.plotdict[k] for k in ["add_nicks", "add_result_nicks","add_scale_factors"]]
		):
			add_histogram = None
			log.debug("AddHistograms: "+add_result_nick+" = "+(" + ".join([str(scale)+"*"+nick for nick, scale in zip(add_nicks, add_scale_factors)])))
			for nick, add_scale_factor in zip(add_nicks, add_scale_factors):
				root_object = plotData.plotdict["root_objects"][nick]
				assert(isinstance(root_object, ROOT.TH1))
				if add_histogram is None:
					add_histogram = root_object.Clone(
							"add_" + hashlib.md5("_".join([str(add_nicks), add_result_nick])).hexdigest()
					)
					add_histogram.Scale(add_scale_factor)
					add_histogram.SetDirectory(0)
				else:
					add_histogram.Add(root_object, add_scale_factor)
					add_histogram.SetDirectory(0)

			plotData.plotdict["root_objects"][add_result_nick] = add_histogram

