# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT
import math

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.analysis_modules.scaleerrors as scaleerrors

import numpy as np

class IsomorphicMapping(analysisbase.AnalysisBase):
	"""Add isomorphic mapping of histograms"""

	def modify_argument_parser(self, parser, args):
		super(IsomorphicMapping, self).modify_argument_parser(parser, args)

		self.isomap_options = parser.add_argument_group("{} options".format(self.name()))
		self.isomap_options.add_argument("--isomap-mc-nicks", nargs="+",
				help="Nick names for the MC samples. Multiple nicks (whitespace separated) will be summed up before calculating the isomorphic mapping. This will be calibrated (i.e. shifted) to fit data.")
		self.isomap_options.add_argument("--isomap-data-nicks", nargs="+",
				help="Nick names for the data sample. Multiple nicks (whitespace separated) will be summed up before calculating the isomorphic mapping. This will be used to calibrate the MC.")
		self.isomap_options.add_argument("--isomap-result-nicks", nargs="+",
				help="Nick names for the resulting graphs.")
		self.isomap_options.add_argument("--n-quantile-positions", default=10000, type=int,
				help="Choose how many quantile position will be probed for. [Default: $(default)s]")


	def prepare_args(self, parser, plotData):
		super(IsomorphicMapping, self).prepare_args(parser, plotData)

		self._plotdict_keys = ["isomap_mc_nicks", "isomap_data_nicks", "isomap_result_nicks"]

		for key in self._plotdict_keys:
			# if (len(plotData.plotdict[key]) == 1) and (plotData.plotdict[key][0] is None):
			if (plotData.plotdict[key] is None):
				plotData.plotdict[key] = plotData.plotdict["nicks"]

		self.prepare_list_args(plotData, self._plotdict_keys)
		for index, (isomap_mc_nick, isomap_data_nick, isomap_result_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["isomap_mc_nicks", "isomap_data_nicks", "isomap_result_nicks"]])):
			plotData.plotdict["isomap_mc_nicks"][index] = isomap_mc_nick.split()
			plotData.plotdict["isomap_data_nicks"][index] = isomap_data_nick.split()

			if isomap_result_nick is None:
				plotData.plotdict["isomap_result_nicks"][index] = "isomap_{MC}_over_{DATA}".format(
						MC="_".join(plotData.plotdict["isomap_mc_nicks"][index]),
						DATA="_".join(plotData.plotdict["isomap_data_nicks"][index]))

			if not plotData.plotdict["isomap_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].append(plotData.plotdict["isomap_result_nicks"][index])



	def run(self, plotData=None):
		super(IsomorphicMapping, self).run(plotData)

		normalise_mc = 0.
		normalise_data = 0.

		isomap_histogram = {}
		cumulative_histogram = {}
		norm_factor = {}

		nick_collection_mc = [plotData.plotdict["isomap_mc_nicks"]][0][0]
		nick_collection_data = [plotData.plotdict["isomap_data_nicks"]][0][0]
		nick_collection = nick_collection_mc + nick_collection_data
		isomap_result_nick = plotData.plotdict["isomap_result_nicks"][-1]
		histogram_mc = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick] for nick in nick_collection_mc])
		histogram_data = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick] for nick in nick_collection_data])

		n_bins = plotData.plotdict["n_quantile_positions"]

		quantile_positions = np.linspace(0, 1, n_bins)

		x_value_mc = np.array([ROOT.Double(0)] * n_bins)
		histogram_mc.GetQuantiles(n_bins, x_value_mc, quantile_positions)

		x_value_data = np.array([ROOT.Double(0)] * n_bins)
		histogram_data.GetQuantiles(n_bins, x_value_data, quantile_positions)

		isomap_tgraph = ROOT.TGraph(n_bins, x_value_mc, x_value_data)
		isomap_mc_tgraph = ROOT.TGraph(n_bins , x_value_mc, x_value_mc)
		isomap_diff_tgraph = ROOT.TGraph(n_bins, x_value_mc, x_value_data - x_value_mc)

		if hasattr(isomap_histogram, "SetDirectory"): isomap_histogram.SetDirectory(0)
		isomap_tgraph.SetTitle("")
		plotData.plotdict["root_objects"][isomap_result_nick] = isomap_tgraph
		plotData.plotdict["root_objects"][isomap_result_nick + "_mc"] = isomap_mc_tgraph
		plotData.plotdict["root_objects"]["difference"] = isomap_diff_tgraph
