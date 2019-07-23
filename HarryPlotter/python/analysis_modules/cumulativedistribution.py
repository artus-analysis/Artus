# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import hashlib
import math

import Artus.HarryPlotter.analysisbase as analysisbase

class CumulativeDistribution(analysisbase.AnalysisBase):
	"""Derive cumulative distribution of (multi-dimensional) histograms. This module takes under-/over-flow bins into account"""
	
	def __init__(self):
		super(CumulativeDistribution, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(CumulativeDistribution, self).modify_argument_parser(parser, args)
		
		self.cumulative_distribution_options = parser.add_argument_group("{} options".format(self.name()))
		self.cumulative_distribution_options.add_argument("--cumulative-input-nicks", nargs="+", default=[None],
				help="Nick names of input histograms. [Default: all]")
		self.cumulative_distribution_options.add_argument("--axis-ordering", default="zyx",
				choices=["xyz", "xzy", "yxz", "zxy", "yzx", "zyx"],
				help="Axis ordering for multi-dimensional input histograms. [Default: %(default)s]")
		self.cumulative_distribution_options.add_argument("--axis-sense", default=1, type=int,
				choices=[1, -1],
				help="Sense for walking through bins. 1: ascending, -1: descending. [Default: %(default)s]")
		self.cumulative_distribution_options.add_argument("--cumulative-output-nicks", nargs="+", default=[None],
				help="Nick names of resulting cumulative histograms. [Default: in-place]")
		
	def prepare_args(self, parser, plotData):
		super(CumulativeDistribution, self).prepare_args(parser, plotData)
		
		self._plotdict_keys = ["cumulative_input_nicks", "cumulative_output_nicks"]
		
		for key in self._plotdict_keys:
			if (len(plotData.plotdict[key]) == 1) and (plotData.plotdict[key][0] is None):
				plotData.plotdict[key] = plotData.plotdict["nicks"]
		
		self.prepare_list_args(plotData, self._plotdict_keys)
		
		for cumulative_input_nick, cumulative_output_nick in zip(*[plotData.plotdict[key] for key in self._plotdict_keys]):
			if not cumulative_output_nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
					plotData.plotdict["nicks"].index(cumulative_input_nick)+1,
					cumulative_output_nick
				)
	
	def run(self, plotData=None):
		super(CumulativeDistribution, self).run(plotData)
		
		for cumulative_input_nick, cumulative_output_nick in zip(*[plotData.plotdict[key] for key in self._plotdict_keys]):
			input_histogram = plotData.plotdict["root_objects"][cumulative_input_nick]
			output_histogram = input_histogram.Clone("cumulative_distribution_" + hashlib.md5("_".join([input_histogram.GetName(), cumulative_input_nick, cumulative_output_nick])).hexdigest())
			output_histogram.Reset()
			
			if not isinstance(input_histogram, ROOT.TH1):
				log.warning("CumulativeDistribution module can only handle histograms. Nick \""+cumulative_input_nick+"\" does match this requirement!")
				plotData.plotdict["root_objects"][cumulative_output_nick] = plotData.plotdict["root_objects"][cumulative_input_nick]
			else:
				n_bins = {
					"x" : input_histogram.GetNbinsX(),
					"y" : input_histogram.GetNbinsY(),
					"z" : input_histogram.GetNbinsZ(),
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
							global_bin = input_histogram.GetBin(x_bin, y_bin, z_bin)
							
							current_sum += input_histogram.GetBinContent(global_bin)
							current_sum_error = math.sqrt(pow(current_sum_error, 2) + pow(input_histogram.GetBinError(global_bin), 2))
							
							output_histogram.SetBinContent(global_bin, current_sum)
							output_histogram.SetBinError(global_bin, current_sum_error)
				
				plotData.plotdict.setdefault("root_objects", {})[cumulative_output_nick] = output_histogram

