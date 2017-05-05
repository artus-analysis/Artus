# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy
import hashlib


import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.analysis_modules.scaleerrors as scaleerrors
import ROOT
import numpy as np

class Divide(analysisbase.AnalysisBase):
	"""Divide selected histogram(s) (numerator) by other histogram(s) (denominator).
		Histograms are not divided in-place, but copies are created (result-nicks)."""

	def modify_argument_parser(self, parser, args):
		super(Divide, self).modify_argument_parser(parser, args)

		self.divide_options = parser.add_argument_group("{} options".format(self.name()))
		self.divide_options.add_argument("--divide-numerator-nicks", type=str, nargs="+",
				help="Nick names for the numerators of the divide.")
		self.divide_options.add_argument("--divide-denominator-nicks", type=str, nargs="+",
				help="Nick names for the denominator of the divide.")
		self.divide_options.add_argument("--divide-result-nicks", type=str, nargs="+",
				help="Nick names for the resulting divide graphs.")
		self.divide_options.add_argument(
				"--divide-tgraphs", nargs="+", type="bool", default=[False],
				help="Do TGraph division instead of implicite conversion to Histogram.")
		self.divide_options.add_argument(
                                "--divide-denominator-no-errors", nargs="+", type="bool", default=[False],
                                help="Remove errors from denominator histo before division.")
		
	def prepare_args(self, parser, plotData):
		super(Divide, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["divide_numerator_nicks", "divide_denominator_nicks", "divide_result_nicks", "divide_tgraphs", "divide_denominator_no_errors"])
		self.auto_set_arguments(plotData, ["divide_numerator_nicks", "divide_denominator_nicks"],
				"divide_result_nicks", "divide")
    	
	def divide_tgraph(self, graph1, graph2):
    		assert (graph1.GetN() == graph2.GetN())
    		for i in xrange(graph1.GetN()):
    	    		graph1X, graph1Y = ROOT.Double(0), ROOT.Double(0)
    	    		graph1.GetPoint(i, graph1X, graph1Y)
    	    		graph2X, graph2Y = ROOT.Double(0), ROOT.Double(0)
    	    		graph2.GetPoint(i, graph2X, graph2Y)
    	    		graph1.SetPoint(i, graph1X, graph1Y / graph2Y if graph2Y != 0. else 0.)
    	    		if isinstance(graph1, ROOT.TGraphAsymmErrors):
    	      		 	graph1.SetPointEYlow(i, np.abs(graph1.GetErrorYlow(i) / graph2Y) if graph2Y != 0. else 0.)
    	   			graph1.SetPointEYhigh(i, np.abs(graph1.GetErrorYhigh(i) / graph2Y) if graph2Y != 0. else 0.)
    	 	  	else:
				graph1.SetPointError(i, 0.0, np.abs(graph1.GetErrorYlow(i) / graph2Y) if graph2Y != 0. else 0.)

	def run(self, plotData=None):
		super(Divide, self).run(plotData)

		for numerator, denominator, new, tgraph, denominator_no_errors in zip(
			plotData.plotdict["divide_numerator_nicks"],
			plotData.plotdict["divide_denominator_nicks"],
			plotData.plotdict["divide_result_nicks"],
			plotData.plotdict["divide_tgraphs"],
			plotData.plotdict["divide_denominator_no_errors"]
		):
			log.debug("Dividing %s by %s (new nick: %s)" % (numerator, denominator, new))
			new_name = "histogram_" + hashlib.md5("_".join([numerator, denominator, new])).hexdigest()
			numerator_histo = plotData.plotdict["root_objects"][numerator].Clone('numerator_'+new_name)
			denominator_histo = plotData.plotdict["root_objects"][denominator].Clone('denominator_'+new_name)
			if(tgraph):
				if isinstance(numerator_histo,ROOT.TGraph) and isinstance(denominator_histo,ROOT.TGraph):
					new_histo = plotData.plotdict["root_objects"][numerator].Clone(new_name)
					self.divide_tgraph(new_histo, denominator_histo)
				else:
					log.error("Divide: Chose TGraph division but input is no TGraph!")
			else:
				if(denominator_no_errors):
					scaleerrors.ScaleErrors.scale_errors(denominator_histo)
				new_histo = roottools.RootTools.to_histogram(plotData.plotdict["root_objects"][numerator]).Clone(new_name)
				new_histo.Divide(roottools.RootTools.to_histogram(denominator_histo))
			plotData.plotdict["root_objects"][new] = new_histo
			plotData.plotdict["nicks"].append(new)
