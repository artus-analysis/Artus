# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import hashlib

import Artus.HarryPlotter.analysisbase as analysisbase

class UnrollHistogram(analysisbase.AnalysisBase):
	"""Convert multi-dimensional histogram into concatenation of one dimensional histograms"""
	
	def __init__(self):
		super(UnrollHistogram, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(UnrollHistogram, self).modify_argument_parser(parser, args)
		
		self.unroll_histogram_options = parser.add_argument_group("{} options".format(self.name()))
		self.unroll_histogram_options.add_argument("--unroll-input-nicks", nargs="+", default=[None],
				help="Nick names of multi-dimensional input histograms. [Default: all]")
		self.unroll_histogram_options.add_argument("--unroll-ordering", default="zyx",
				choices=["xyz", "xzy", "yxz", "zxy", "yzx", "zyx"],
				help="Axis ordering for unrolling. [Default: %(default)s]")
		self.unroll_histogram_options.add_argument("--unroll-output-nicks", nargs="+", default=[None],
				help="Nick names of resulting 1D histograms. [Default: unroll in-place]")
		
	def prepare_args(self, parser, plotData):
		super(UnrollHistogram, self).prepare_args(parser, plotData)
		
		self._plotdict_keys = ["unroll_input_nicks", "unroll_output_nicks"]
		
		for key in self._plotdict_keys:
			if (len(plotData.plotdict[key]) == 1) and (plotData.plotdict[key][0] is None):
				plotData.plotdict[key] = plotData.plotdict["nicks"]
		
		self.prepare_list_args(plotData, self._plotdict_keys)
		
		for unroll_input_nick, unroll_output_nick in zip(*[plotData.plotdict[key] for key in self._plotdict_keys]):
			if not unroll_output_nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(
					plotData.plotdict["nicks"].index(unroll_input_nick)+1,
					unroll_output_nick
				)
	
	def run(self, plotData=None):
		super(UnrollHistogram, self).run(plotData)
		
		for unroll_input_nick, unroll_output_nick in zip(*[plotData.plotdict[key] for key in self._plotdict_keys]):
			input_histogram = plotData.plotdict["root_objects"][unroll_input_nick]
			
			if (not isinstance(input_histogram, ROOT.TH1)) or (input_histogram.GetDimension() == 1):
				plotData.plotdict["root_objects"][unroll_output_nick] = plotData.plotdict["root_objects"][unroll_input_nick]
			else:
				n_bins = {
					"x" : input_histogram.GetNbinsX(),
					"y" : input_histogram.GetNbinsY(),
					"z" : input_histogram.GetNbinsZ(),
				}
				current_bin = {
					"x" : 0,
					"y" : 0,
					"z" : 0,
				}
				
				n_output_bins = reduce(lambda a, b: a*b, n_bins.values())
				output_histogram = ROOT.TH1D(
						"histogram_" + hashlib.md5("_".join([input_histogram.GetName(), unroll_input_nick, unroll_output_nick])).hexdigest(), "",
						n_output_bins, 0, n_output_bins
				)
				
				output_bin = 1
				for bin_axis_1 in xrange(1, n_bins[plotData.plotdict["unroll_ordering"][0]]+1):
					current_bin[plotData.plotdict["unroll_ordering"][0]] = bin_axis_1
					
					for bin_axis_2 in xrange(1, n_bins[plotData.plotdict["unroll_ordering"][1]]+1):
						current_bin[plotData.plotdict["unroll_ordering"][1]] = bin_axis_2
						
						for bin_axis_3 in xrange(1, n_bins[plotData.plotdict["unroll_ordering"][2]]+1):
							current_bin[plotData.plotdict["unroll_ordering"][2]] = bin_axis_3
							
							x_bin = current_bin["x"]
							y_bin = current_bin["y"]
							z_bin = current_bin["z"]
							global_bin = input_histogram.GetBin(x_bin, y_bin, z_bin)
							
							output_histogram.SetBinContent(output_bin, input_histogram.GetBinContent(global_bin))
							output_histogram.SetBinError(output_bin, input_histogram.GetBinError(global_bin))
							output_bin += 1
				
				output_histogram.SetEntries(input_histogram.GetEntries())
				plotData.plotdict.setdefault("root_objects", {})[unroll_output_nick] = output_histogram

