# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import array
import hashlib

import Artus.HarryPlotter.analysisbase as analysisbase


class ContourFromHistogram(analysisbase.AnalysisBase):
	""" Retrieve contour graphs from 2D histograms."""

	def modify_argument_parser(self, parser, args):
		super(ContourFromHistogram, self).modify_argument_parser(parser, args)

		self.contour_options = parser.add_argument_group("Contour options")
		self.contour_options.add_argument(
				"--2d-histogram-nicks", type=str, nargs="+",
				help="Nick names of the 2D input histograms."
		)
		self.contour_options.add_argument(
				"--contour-thresholds", type=str, nargs="+",
				help="Thresholds for the contours. Multiple contours for one histogram can be specified in one argument separated by a whitespace."
		)
		self.contour_options.add_argument(
				"--contour-graph-nicks", type=str, nargs="+", default=[None],
				help="Nick names for the resulting graphs. If more than one threshold is specified, \"_<index>\" is appended to the nick name."
		)

	def prepare_args(self, parser, plotData):
		super(ContourFromHistogram, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["2d_histogram_nicks", "contour_thresholds", "contour_graph_nicks"])
		
		for index, (histogram_nick, contour_thresholds, contour_graph_nicks) in enumerate(zip(*[plotData.plotdict[k] for k in ["2d_histogram_nicks", "contour_thresholds", "contour_graph_nicks"]])):
			contour_thresholds = [float(threshold) for threshold in contour_thresholds.split()]
			plotData.plotdict["contour_thresholds"][index] = contour_thresholds
		
			if contour_graph_nicks is None:
				contour_graph_nicks = "contour_"+histogram_nick
			contour_graph_nicks = contour_graph_nicks.split()
			exapand_all = (len(contour_graph_nicks) == 1 and len(contour_thresholds) > 1)
			contour_graph_nicks = (contour_graph_nicks+([contour_graph_nicks[0]]*len(contour_thresholds)))[:len(contour_thresholds)]
			contour_graph_nicks = ["%s_%d" % (n, i) if exapand_all or (i > 0 and n == contour_graph_nicks[0]) else n for i, n in enumerate(contour_graph_nicks)]
			plotData.plotdict["contour_graph_nicks"][index] = contour_graph_nicks

	def run(self, plotData=None):
		super(ContourFromHistogram, self).run(plotData)

		for index, (histogram_nick, contour_thresholds, contour_graph_nicks) in enumerate(zip(*[plotData.plotdict[k] for k in ["2d_histogram_nicks", "contour_thresholds", "contour_graph_nicks"]])):
			
			histogram = plotData.plotdict["root_objects"][histogram_nick]
			
			contours = array.array("d", contour_thresholds)
			histogram.SetContour(len(contours), contours)
			
			# without this canvas, no contours can be retrieved
			# https://root.cern.ch/root/html/tutorials/hist/ContourList.C.html#71
			tmp_canvas = ROOT.TCanvas("tmp_canvas", "");
			histogram.Draw("CONT Z LIST")
			tmp_canvas.Update()
			contours_list = ROOT.gROOT.GetListOfSpecials().FindObject("contours")
			
			for index, (contour_threshold, contour_graph_nick) in enumerate(zip(contour_thresholds, contour_graph_nicks)):
				contours_list_for_threshold = contours_list.At(index)
				
				n_graphs = contours_list_for_threshold.GetSize()
				for graph_index in xrange(n_graphs):
					if n_graphs > 1:
						tmp_contour_graph_nick = "%s_%d" % (contour_graph_nick, graph_index)
					if not tmp_contour_graph_nick in plotData.plotdict["nicks"]:
						plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(histogram_nick), tmp_contour_graph_nick)
					
					graph = contours_list_for_threshold.At(graph_index)
					graph.SetTitle("")
					plotData.plotdict["root_objects"][tmp_contour_graph_nick] = graph
					log.debug("Retrived contour \"%s\" for threshold %f from histogram \"%s\"." % (tmp_contour_graph_nick, contour_threshold, histogram_nick))

