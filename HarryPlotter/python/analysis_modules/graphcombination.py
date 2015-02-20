# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import array
import hashlib

import Artus.HarryPlotter.analysisbase as analysisbase


class GraphCombination(analysisbase.AnalysisBase):
	""" Combine x/y values of multiple TGraphError objects into TGraphAsymmErrors."""

	def modify_argument_parser(self, parser, args):
		super(GraphCombination, self).modify_argument_parser(parser, args)

		self.graph_combination_options = parser.add_argument_group("Graph combination options")
		self.graph_combination_options.add_argument(
				"--graph-values-nicks", type=str, nargs="+", required=True,
				help="Nick names for graphs with the x-/y-values for the x-/y-values of the resulting graph."
		)
		self.graph_combination_options.add_argument(
				"--graph-low-errors-nicks", type=str, nargs="+", default=[None],
				help="Nick names for graphs with the x-/y-errors for the lower x-/y-errors of the resulting graph."
		)
		self.graph_combination_options.add_argument(
				"--graph-high-errors-nicks", type=str, nargs="+", default=[None],
				help="Nick names for graphs with the x-/y-errors for the upper x-/y-errors of the resulting graph."
		)
		self.graph_combination_options.add_argument(
				"--graph-error-modes", type=str, nargs="+", default=["rel"],
				choices=["rel", "abs"],
				help="Error mode."
		)
		self.graph_combination_options.add_argument(
				"--graph-result-nicks", type=str, nargs="+", default=[None],
				help="Nick names for the resulting TGraphAsymmErrors."
		)

	def prepare_args(self, parser, plotData):
		super(GraphCombination, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["graph_values_nicks", "graph_low_errors_nicks", "graph_high_errors_nicks", "graph_error_modes", "graph_result_nicks"])
		
		for index, (graph_values_nick, graph_low_errors_nick, graph_high_errors_nick, graph_result_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["graph_values_nicks", "graph_low_errors_nicks", "graph_high_errors_nicks", "graph_result_nicks"]])):
			if graph_result_nick is None:
				graph_result_nick = "_".join([graph_values_nick, graph_low_errors_nick, graph_high_errors_nick])
				plotData.plotdict["graph_result_nicks"][index] = graph_result_nick
			if not graph_result_nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(graph_values_nick), graph_result_nick)

	def run(self, plotData=None):
		super(GraphCombination, self).run(plotData)

		for index, (graph_values_nick, graph_low_errors_nick, graph_high_errors_nick, graph_error_mode, graph_result_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["graph_values_nicks", "graph_low_errors_nicks", "graph_high_errors_nicks", "graph_error_modes", "graph_result_nicks"]])):
			graph_values = plotData.plotdict["root_objects"].get(graph_values_nick)
			assert(isinstance(graph_values, ROOT.TGraph))
			n_points = graph_values.GetN()
			x_values = graph_values.GetX()
			x_values = array.array("d", [x_values[i] for i in xrange(n_points)])
			y_values = graph_values.GetY()
			y_values = array.array("d", [y_values[i] for i in xrange(n_points)])
			
			graph_low_errors = plotData.plotdict["root_objects"].get(graph_low_errors_nick)
			assert(graph_low_errors is None or isinstance(graph_low_errors, ROOT.TGraph))
			x_errors_low = array.array("d", [0.0]*n_points)
			y_errors_low = array.array("d", [0.0]*n_points)
			if graph_low_errors:
				assert(graph_low_errors.GetN() == n_points)
				if isinstance(graph_low_errors, ROOT.TGraphErrors):
					x_errors_low = graph_low_errors.GetEX()
					y_errors_low = graph_low_errors.GetEY()
				else:
					x_errors_low = graph_low_errors.GetX()
					y_errors_low = graph_low_errors.GetY()
				x_errors_low = array.array("d", [x_errors_low[i] for i in xrange(graph_low_errors.GetN())])
				y_errors_low = array.array("d", [y_errors_low[i] for i in xrange(graph_low_errors.GetN())])
			
			graph_high_errors = plotData.plotdict["root_objects"].get(graph_high_errors_nick)
			assert(graph_high_errors is None or isinstance(graph_high_errors, ROOT.TGraph))
			x_errors_high = array.array("d", [0.0]*n_points)
			y_errors_high = array.array("d", [0.0]*n_points)
			if graph_high_errors:
				assert(graph_high_errors.GetN() == n_points)
				if isinstance(graph_high_errors, ROOT.TGraphErrors):
					x_errors_high = graph_high_errors.GetEX()
					y_errors_high = graph_high_errors.GetEY()
				else:
					x_errors_high = graph_high_errors.GetX()
					y_errors_high = graph_high_errors.GetY()
				x_errors_high = array.array("d", [x_errors_high[i] for i in xrange(graph_high_errors.GetN())])
				y_errors_high = array.array("d", [y_errors_high[i] for i in xrange(graph_high_errors.GetN())])
			
			if graph_error_mode == "abs":
				if graph_low_errors:
					x_errors_low = array.array("d", [abs(x-xe) for x, xe in zip(x_values, x_errors_low)])
					y_errors_low = array.array("d", [abs(y-ye) for y, ye in zip(y_values, y_errors_low)])
				if graph_high_errors:
					x_errors_high = array.array("d", [abs(x-xe) for x, xe in zip(x_values, x_errors_high)])
					y_errors_high = array.array("d", [abs(y-ye) for y, ye in zip(y_values, y_errors_high)])
			
			graph_result = ROOT.TGraphAsymmErrors(
					n_points,
					x_values, y_values,
					x_errors_low, x_errors_high,
					y_errors_low, y_errors_high
			)
			graph_result.SetName("graph_" + hashlib.md5("_".join([str(setting) for setting in [graph_values_nick, graph_low_errors_nick, graph_high_errors_nick, graph_error_mode, graph_result_nick]])).hexdigest())
			graph_result.SetTitle("")
			plotData.plotdict["root_objects"][graph_result_nick] = graph_result

