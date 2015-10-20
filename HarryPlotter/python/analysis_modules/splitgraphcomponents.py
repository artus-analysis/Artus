# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import numpy

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class SplitGraphComponents(analysisbase.AnalysisBase):
	"""Fill components of graphs (central values or error bands) into new graphs."""
	
	def __init__(self):
		super(SplitGraphComponents, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(SplitGraphComponents, self).modify_argument_parser(parser, args)

		self.SplitGraphComponents_options = parser.add_argument_group("{0} options".format(self.name()))
		self.SplitGraphComponents_options.add_argument(
				"--split-graph-nicks", type=str, nargs="+",
				help="Nick names of the graphs to be split. The resulting graphs will have the same nick plus the component identifier."
		)
		self.SplitGraphComponents_options.add_argument(
				"--split-graph-components", nargs="+", default=["yelow", "yehigh"],
				choices=["xelow", "x", "xehigh", "yelow", "y", "yehigh", "zelow", "z", "zehigh"],
				help="Graph components to separate. [Default: %(default)s]")
	
	def prepare_args(self, parser, plotData):
		super(SplitGraphComponents, self).prepare_args(parser, plotData)
		for nick in plotData.plotdict["split_graph_nicks"]:
			for graph_component in plotData.plotdict["split_graph_components"]:
				new_nick = nick+"_"+graph_component
				if not new_nick in plotData.plotdict["nicks"]:
					plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(nick), new_nick)
	
	def run(self, plotData=None):
		super(SplitGraphComponents, self).run(plotData)
		
		for nick in plotData.plotdict["split_graph_nicks"]:
			graph = plotData.plotdict["root_objects"][nick]
			assert any([isinstance(graph, graph_class) for graph_class in [ROOT.TGraphErrors, ROOT.TGraphAsymmErrors, ROOT.TGraph2DErrors]])
			
			n_points = graph.GetN()
			is_graph_2d = isinstance(graph, ROOT.TGraph2DErrors)
			is_graph_asymm = isinstance(graph, ROOT.TGraphAsymmErrors)
			
			buffers = []
			indices = {}
			
			buffers.append(graph.GetX())
			indices["x"] = len(buffers) - 1
			
			buffers.append(graph.GetY())
			indices["y"] = len(buffers) - 1
			
			if is_graph_2d:
				buffers.append(graph.GetZ())
				indices["z"] = len(buffers) - 1
			
			if is_graph_asymm:
				buffers.append(graph.GetEXlow())
				indices["xelow"] = len(buffers) - 1
				buffers.append(graph.GetEXhigh())
				indices["xehigh"] = len(buffers) - 1
			else:
				buffers.append(graph.GetEX())
				indices["xelow"] = len(buffers) - 1
				indices["xehigh"] = len(buffers) - 1
			
			if is_graph_asymm:
				buffers.append(graph.GetEYlow())
				indices["yelow"] = len(buffers) - 1
				buffers.append(graph.GetEYhigh())
				indices["yehigh"] = len(buffers) - 1
			else:
				buffers.append(graph.GetEY())
				indices["yelow"] = len(buffers) - 1
				indices["yehigh"] = len(buffers) - 1
			
			if is_graph_2d:
				buffers.append(graph.GetEZ())
				indices["zelow"] = len(buffers) - 1
				indices["zehigh"] = len(buffers) - 1
			
			values = [numpy.ndarray(n_points, dtype=numpy.double, buffer=buffer) for buffer in buffers]
			zeros = numpy.zeros(n_points)
			
			graph_class = ROOT.TGraphErrors
			if is_graph_2d:
				graph_class = ROOT.TGraph2DErrors
			elif is_graph_asymm:
				graph_class = ROOT.TGraphAsymmErrors
			
			for graph_component in plotData.plotdict["split_graph_components"]:
				new_nick = nick+"_"+graph_component
				
				tmp_values = copy.deepcopy(values)
				
				if "x" in graph_component:
					tmp_values[indices["x"]] = self._get_new_component(
							tmp_values[indices["x"]],
							tmp_values[indices[graph_component]],
							(-1 if "low" in graph_component else (1 if "high" in graph_component else 0))
					)
					tmp_values[indices["xelow"]] = zeros
					tmp_values[indices["xehigh"]] = zeros
				elif "y" in graph_component:
					tmp_values[indices["y"]] = self._get_new_component(
							tmp_values[indices["y"]],
							tmp_values[indices[graph_component]],
							(-1 if "low" in graph_component else (1 if "high" in graph_component else 0))
					)
					tmp_values[indices["yelow"]] = zeros
					tmp_values[indices["yehigh"]] = zeros
				elif "z" in graph_component:
					tmp_values[indices["z"]] = self._get_new_component(
							tmp_values[indices["z"]],
							tmp_values[indices[graph_component]],
							(-1 if "low" in graph_component else (1 if "high" in graph_component else 0))
					)
					tmp_values[indices["zelow"]] = zeros
					tmp_values[indices["zehigh"]] = zeros
				
				new_graph = graph_class(n_points, *tmp_values)
				new_graph.SetName(graph.GetName()+"_"+graph_component)
				plotData.plotdict["root_objects"][new_nick] = new_graph
			
	def _get_new_component(self, central, new, shift=0):
		if shift == 0:
			return new
		else:
			return array.array("d", [central+(shift*error) for central, error in zip(central, new)])

