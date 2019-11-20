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
		self.isomap_options.add_argument("--axis-ordering", default="zyx",
				choices=["xyz", "xzy", "yxz", "zxy", "yzx", "zyx"],
				help="Axis ordering for multi-dimensional input histograms used for the cumulative distributions. [Default: %(default)s]")
		self.isomap_options.add_argument("--axis-sense", default=1, type=int,
				choices=[1, -1],
				help="Sense for walking through bins when calculating the cumulative distributions. 1: ascending, -1: descending. [Default: %(default)s]")

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

		isomap_tgraph = ROOT.TGraph()
		isomap_histogram = {}
		cumulative_histogram = {}
		norm_factor = {}

		nick_collection_mc = [plotData.plotdict["isomap_mc_nicks"]][0][0]
		nick_collection_data = [plotData.plotdict["isomap_data_nicks"]][0][0]
		nick_collection = nick_collection_mc + nick_collection_data

		for cumulative_input_nick in nick_collection:
			isomap_histogram[cumulative_input_nick] = plotData.plotdict["root_objects"][cumulative_input_nick]
			cumulative_histogram[cumulative_input_nick] = isomap_histogram[cumulative_input_nick].Clone("cumulative_distribution_" + hashlib.md5("_".join([isomap_histogram[cumulative_input_nick].GetName(), cumulative_input_nick, "cumulative_output_nick"])).hexdigest())
			cumulative_histogram[cumulative_input_nick].Reset()

			n_bins = {
				"x" : isomap_histogram[cumulative_input_nick].GetNbinsX(),
				"y" : isomap_histogram[cumulative_input_nick].GetNbinsY(),
				"z" : isomap_histogram[cumulative_input_nick].GetNbinsZ(),
			}
			bin_ranges = {}
			for axis, bins in n_bins.iteritems():
				bin_ranges[axis] = (1, 2) if bins == 1 else (0, bins+2)

			current_bin = {
				"x" : 0,
				"y" : 0,
				"z" : 0,
			}
			current_sum = 0.0
			current_sum_error = 0.0

			for bin_axis_1 in range(*bin_ranges[plotData.plotdict["axis_ordering"][0]])[::plotData.plotdict["axis_sense"]]:
				current_bin[plotData.plotdict["axis_ordering"][0]] = bin_axis_1

				for bin_axis_2 in range(*bin_ranges[plotData.plotdict["axis_ordering"][1]])[::plotData.plotdict["axis_sense"]]:
					current_bin[plotData.plotdict["axis_ordering"][1]] = bin_axis_2

					for bin_axis_3 in range(*bin_ranges[plotData.plotdict["axis_ordering"][2]])[::plotData.plotdict["axis_sense"]]:
						current_bin[plotData.plotdict["axis_ordering"][2]] = bin_axis_3

						x_bin = current_bin["x"]
						y_bin = current_bin["y"]
						z_bin = current_bin["z"]
						global_bin = isomap_histogram[cumulative_input_nick].GetBin(x_bin, y_bin, z_bin)

						current_sum += isomap_histogram[cumulative_input_nick].GetBinContent(global_bin)
						current_sum_error = math.sqrt(pow(current_sum_error, 2) + pow(isomap_histogram[cumulative_input_nick].GetBinError(global_bin), 2))

						cumulative_histogram[cumulative_input_nick].SetBinContent(global_bin, current_sum)
						cumulative_histogram[cumulative_input_nick].SetBinError(global_bin, current_sum_error)
			norm_factor[cumulative_input_nick] = current_sum

		norm_mc = sum([norm_factor[key] for key in nick_collection_mc])
		cumulative_histogram_mc = roottools.RootTools.add_root_histograms(*[cumulative_histogram[nick] for nick in nick_collection_mc], scale_factors= [1./norm_mc]*len(nick_collection_mc))

		norm_data = sum([norm_factor[key] for key in nick_collection_data])
		cumulative_histogram_data = roottools.RootTools.add_root_histograms(*[cumulative_histogram[nick] for nick in nick_collection_data], scale_factors= [1./norm_data]*len(nick_collection_data))

		# Calculate isomap
		isomap_result_nick = plotData.plotdict["isomap_result_nicks"][0]

		for bin_axis_1 in range(*bin_ranges[plotData.plotdict["axis_ordering"][0]])[::plotData.plotdict["axis_sense"]]:
			current_bin[plotData.plotdict["axis_ordering"][0]] = bin_axis_1

			for bin_axis_2 in range(*bin_ranges[plotData.plotdict["axis_ordering"][1]])[::plotData.plotdict["axis_sense"]]:
				current_bin[plotData.plotdict["axis_ordering"][1]] = bin_axis_2

				for bin_axis_3 in range(*bin_ranges[plotData.plotdict["axis_ordering"][2]])[::plotData.plotdict["axis_sense"]]:
					current_bin[plotData.plotdict["axis_ordering"][2]] = bin_axis_3

					x_bin = current_bin["x"]
					y_bin = current_bin["y"]
					z_bin = current_bin["z"]
					global_bin = cumulative_histogram_mc.GetBin(x_bin, y_bin, z_bin)

					x_value_mc = cumulative_histogram_mc.GetBinCenter(global_bin)
					y_value_mc = cumulative_histogram_mc.GetBinContent(global_bin)

					x_value_data = np.array([ROOT.Double(0)])
					cumulative_histogram_data.GetQuantiles(1, x_value_data, np.array([y_value_mc]))
					isomap_tgraph.SetPoint(global_bin, x_value_mc, x_value_data[0])

		if hasattr(isomap_histogram, "SetDirectory"): isomap_histogram.SetDirectory(0)
		isomap_tgraph.SetTitle("")
		plotData.plotdict["root_objects"][isomap_result_nick] = isomap_tgraph
