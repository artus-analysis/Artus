# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import functools
import hashlib
import math

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.Utility.tools as tools


class LikelihoodScan(analysisbase.AnalysisBase):
	"""Create graphs indicating likelihood crossings."""
	
	def __init__(self):
		super(LikelihoodScan, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(LikelihoodScan, self).modify_argument_parser(parser, args)

		self.likelihood_scan_options = parser.add_argument_group("{} options".format(self.name()))
		self.likelihood_scan_options.add_argument("--likelihood-graph-nicks", nargs="+", default=None,
				help="Nicks of likelihood scan graphs. [Default: all]")
		self.likelihood_scan_options.add_argument("--likelihood-crossings", nargs="+", default=["0 1 4"],
				help="Likelihood values for which to search x-axis values. [Default: %(default)s]")
		self.likelihood_scan_options.add_argument("--likelihood-scan-result-nicks", nargs="+", default=None,
				help="Nicks of resulting graphs showing the likelihood crossings. [Default: same as input]")

	def prepare_args(self, parser, plotData):
		super(LikelihoodScan, self).prepare_args(parser, plotData)
		
		if plotData.plotdict["likelihood_graph_nicks"] is None:
			plotData.plotdict["likelihood_graph_nicks"] = plotData.plotdict["nicks"]
		
		if plotData.plotdict["likelihood_scan_result_nicks"] is None:
			plotData.plotdict["likelihood_scan_result_nicks"] = plotData.plotdict["likelihood_graph_nicks"]
		
		self.prepare_list_args(plotData, ["likelihood_graph_nicks", "likelihood_crossings", "likelihood_scan_result_nicks"])
		
		for index, (likelihood_graph_nick, likelihood_crossings, likelihood_scan_result_nick) in enumerate(zip(*[plotData.plotdict[key] for key in ["likelihood_graph_nicks", "likelihood_crossings", "likelihood_scan_result_nicks"]])):
			likelihood_crossings = likelihood_crossings.split()
			likelihood_scan_result_nicks = [likelihood_scan_result_nick+"_"+likelihood_crossing for likelihood_crossing in likelihood_crossings]
			likelihood_crossings = [float(likelihood_crossing) for likelihood_crossing in likelihood_crossings]
			
			plotData.plotdict["likelihood_crossings"][index] = likelihood_crossings
			plotData.plotdict["likelihood_scan_result_nicks"][index] = likelihood_scan_result_nicks
			for nick in likelihood_scan_result_nicks[::-1]:
				if not (nick in plotData.plotdict["nicks"]):
					plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(likelihood_graph_nick)+1, nick)

	def run(self, plotData=None):
		super(LikelihoodScan, self).run(plotData)
		
		for likelihood_graph_nick, likelihood_crossings, likelihood_scan_result_nicks in zip(*[plotData.plotdict[key] for key in ["likelihood_graph_nicks", "likelihood_crossings", "likelihood_scan_result_nicks"]]):
			
			likelihood_graph = plotData.plotdict["root_objects"][likelihood_graph_nick]
			if isinstance(likelihood_graph, ROOT.TGraph):
				likelihood_graph.Sort()
				
				for likelihood_crossing, likelihood_scan_result_nick in zip(likelihood_crossings, likelihood_scan_result_nicks):
					x_crossings = LikelihoodScan.find_crossing(likelihood_graph, likelihood_crossing)
					log.debug("Nick \"{nick}\", {n_crossings} crossing(s) at y={y_crossing}: {x_crossings}. Results saved to nick \"result\".".format(
							nick=likelihood_graph_nick,
							n_crossings=str(len(x_crossings)),
							y_crossing=str(likelihood_crossing),
							x_crossings=", ".join([str(x_crossing) for x_crossing in x_crossings]),
							result=likelihood_scan_result_nick
					))
					crossings_graph = LikelihoodScan.crossings_graph(x_crossings, likelihood_crossing if likelihood_crossing!= 0.0 else max(likelihood_crossings))
					plotData.plotdict["root_objects"][likelihood_scan_result_nick] = crossings_graph
				
	@staticmethod
	def find_crossing(graph, y_crossing):
		x_values = graph.GetX()
		x_values = [x_values[index] for index in xrange(graph.GetN())]
		y_values = graph.GetY()
		y_values = [y_values[index] for index in xrange(graph.GetN())]
		xy_values = zip(x_values, y_values)
		
		def eval(graph, x, params):
			return graph.Eval(x[0])
		
		spline = ROOT.TSpline3("spline_"+graph.GetName(), graph)
		spline_function = ROOT.TF1("spline_function_"+graph.GetName(), functools.partial(eval, spline), min(x_values), max(x_values), 1)
		
		x_crossings = []
		for x_value, y_value in xy_values:
			if y_value == y_crossing:
				x_crossings.append(x_value)

		if (y_crossing != 0.0) or (len(x_crossings) == 0):		
			for (x_low, y_low), (x_high, y_high) in zip(xy_values[:-1], xy_values[1:]):
				if (y_low - y_crossing) * (y_high - y_crossing) < 0.0:
					x_crossings.append(spline_function.GetX(y_crossing, x_low, x_high))
		
		if len(x_crossings) == 0:
			x_crossings.append(sorted(xy_values, key=lambda xy: abs(xy[1]-y_crossing))[0][0])
		
		return x_crossings
				
	@staticmethod
	def crossings_graph(x_crossings, y_crossing):
		x_values = array.array("d", tools.flattenList(zip(*([x_crossings]*3))))
		y_values = array.array("d", [y_crossing, 0.0, y_crossing]*len(x_crossings))
		graph = ROOT.TGraph(len(x_values), x_values, y_values)
		graph.SetName("graph_" + hashlib.md5("_".join([str(x_crossings), str(y_crossing)])).hexdigest())
		return graph

