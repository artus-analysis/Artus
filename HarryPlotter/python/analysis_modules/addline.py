# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import array
import hashlib

import Artus.HarryPlotter.analysisbase as analysisbase


class AddLine(analysisbase.AnalysisBase):
	""" Add TGraphs to the ROOT objects for beeing plotted as lines """

	def modify_argument_parser(self, parser, args):
		super(AddLine, self).modify_argument_parser(parser, args)

		self.add_line_options = parser.add_argument_group("Add line options")
		self.add_line_options.add_argument(
				"--x-lines", type=str, nargs="+", default=[None],
				help="X-coordinates for the lines. Coordinates of one line need to be put in one argument separated by a whitespace."
		)
		self.add_line_options.add_argument(
				"--y-lines", type=str, nargs="+", default=[None],
				help="X-coordinates for the lines. Coordinates of one line need to be put in one argument separated by a whitespace."
		)
		self.add_line_options.add_argument(
				"--z-lines", type=str, nargs="+", default=[None],
				help="X-coordinates for the lines. Coordinates of one line need to be put in one argument separated by a whitespace."
		)
		self.add_line_options.add_argument(
				"--line-nicks", type=str, nargs="+", default=[None],
				help="Nick names for the resulting line TGraphs."
		)

	def prepare_args(self, parser, plotData):
		super(AddLine, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["x_lines", "y_lines", "z_lines", "line_nicks"])
		
		for index, (x_lines, y_lines, z_lines, line_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["x_lines", "y_lines", "z_lines", "line_nicks"]])):
			if x_lines is None:
				log.error("Lines need x-coordinates!")
				x_lines = "0.0"
			x_lines = [float(x) for x in x_lines.split()]
			if y_lines is None:
				log.error("Lines need y-coordinates!")
				y_lines = "0.0"
			y_lines = [float(y) for y in y_lines.split()]
			if z_lines is None:
				x_lines, y_lines = zip(*zip(x_lines, y_lines))
			else:
				z_lines = [float(z) for z in z_lines.split()]
				x_lines, y_lines, z_lines = zip(*zip(x_lines, y_lines, z_lines))
			
			if line_nick is None:
				line_nick = "line%d" % index
			if not line_nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].append(line_nick)
			
			plotData.plotdict["x_lines"][index] = x_lines
			plotData.plotdict["y_lines"][index] = y_lines
			plotData.plotdict["z_lines"][index] = z_lines
			plotData.plotdict["line_nicks"][index] = line_nick

	def run(self, plotData=None):
		super(AddLine, self).run(plotData)

		for x_lines, y_lines, z_lines, line_nick in zip(*[plotData.plotdict[k] for k in ["x_lines", "y_lines", "z_lines", "line_nicks"]]):
			x_values = array.array("d", x_lines)
			y_values = array.array("d", y_lines)
			
			graph = None
			if z_lines is None:
				graph = ROOT.TGraph(len(x_values), x_values, y_values)
			else:
				z_values = array.array("d", z_lines)
				graph = ROOT.TGraph(len(x_values), x_values, y_values, z_values)
			graph.SetName("graph_" + hashlib.md5("_".join([str(item) for item in [x_lines, y_lines, z_lines, line_nick]])).hexdigest())
			graph.SetTitle("")
			graph.SetDrawOption("L")
			plotData.plotdict["root_objects"][line_nick] = graph

