# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase


class ShapeYieldMerge(analysisbase.AnalysisBase):
	def __init__(self):
		super(ShapeYieldMerge, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(ShapeYieldMerge, self).modify_argument_parser(parser, args)
		
		self.shape_yield_options = parser.add_argument_group("Shape and yield merging options")
		self.shape_yield_options.add_argument("--shape-nicks", nargs="+", default=[None],
				help="Nick names for the shape extraction.")
		self.shape_yield_options.add_argument("--yield-nicks", nargs="+", default=[None],
				help="Nick names for the yield extraction.")
		self.shape_yield_options.add_argument("--shape-yield-nicks", nargs="+", default=[None],
				help="Nick names for the resulting histograms.")

	def prepare_args(self, parser, plotData):
		super(ShapeYieldMerge, self).prepare_args(parser, plotData)
		
		plotData.plotdict["shape_nicks"] = [nick for nick in plotData.plotdict["shape_nicks"] if nick in plotData.plotdict["root_histos"]]
		plotData.plotdict["yield_nicks"] = [nick for nick in plotData.plotdict["yield_nicks"] if nick in plotData.plotdict["root_histos"]]
		self.prepare_list_args(plotData, ["shape_nicks", "yield_nicks", "shape_yield_nicks"])
		
		for index, nick in enumerate(plotData.plotdict["shape_yield_nicks"]):
			if not nick:
				nick = plotData.plotdict["shape_nicks"][index] + "_" + plotData.plotdict["yield_nicks"][index]
				plotData.plotdict["shape_yield_nicks"][index] = nick
		
	
	def run(self, plotData=None):
		super(ShapeYieldMerge, self).run(plotData)
		
		for index, (shape_nick, yield_nick, shape_yield_nick) in enumerate(zip(plotData.plotdict["shape_nicks"],
		                                                                       plotData.plotdict["yield_nicks"],
		                                                                       plotData.plotdict["shape_yield_nicks"])):
			
			new_histogram_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_histos"][shape_nick].GetName(),
			                                                          plotData.plotdict["root_histos"][yield_nick].GetName()])).hexdigest()
			
			new_histogram = plotData.plotdict["root_histos"][shape_nick].Clone(new_histogram_name)
			new_histogram.Scale(plotData.plotdict["root_histos"][yield_nick].Integral() / plotData.plotdict["root_histos"][shape_nick].Integral())
			
			plotData.plotdict["root_histos"][shape_yield_nick] = new_histogram
			plotData.plotdict.setdefault("nicks", []).append(shape_yield_nick)

