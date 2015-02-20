# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class TGraphFromHistograms(analysisbase.AnalysisBase):
	""" This module takes two (or more) histograms and uses their y-values (and errors) as x,y-pairs for a TGraphErrors."""

	def modify_argument_parser(self, parser, args):
		super(TGraphFromHistograms, self).modify_argument_parser(parser, args)

		self.tgraph_options = parser.add_argument_group("Divide options")
		self.tgraph_options.add_argument("--tgraph-x-nicks", type=str, nargs="*",
			help="Nick names for histograms with the x-values of the graph points.")
		self.tgraph_options.add_argument("--tgraph-y-nicks", type=str, nargs="*",
			help="Nick names for histograms with the y-values of the graph points.")
		self.tgraph_options.add_argument("--tgraph-result-nicks", type=str, nargs="*",
			help="Nick names for the resulting TGraphErrors.")

	def prepare_args(self, parser, plotData):
		super(TGraphFromHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["tgraph_x_nicks", "tgraph_y_nicks", "tgraph_result_nicks"])

	def run(self, plotData=None):
		super(TGraphFromHistograms, self).run(plotData)

		for x, y, new in zip(plotData.plotdict["tgraph_x_nicks"], plotData.plotdict["tgraph_y_nicks"], plotData.plotdict["tgraph_result_nicks"]):
			log.debug("x-values from bin-contents of %s, y-values from bin-contents of %s (new nick: %s)" % (x, y, new))
			plotData.plotdict["root_objects"][new] = ROOT.TGraphErrors()
			plotData.plotdict["nicks"].append(new)

			for i in range(1, plotData.plotdict['root_objects'][x].GetNbinsX()+1):
				plotData.plotdict["root_objects"][new].SetPoint(i, plotData.plotdict['root_objects'][x].GetBinContent(i), plotData.plotdict['root_objects'][y].GetBinContent(i))
				plotData.plotdict["root_objects"][new].SetPointError(i, plotData.plotdict['root_objects'][x].GetBinError(i), plotData.plotdict['root_objects'][y].GetBinError(i))
