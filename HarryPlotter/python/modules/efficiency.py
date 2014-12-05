# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class Efficiency(analysisbase.AnalysisBase):

	def __init__(self):
		super(Efficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Efficiency, self).modify_argument_parser(parser, args)

	def prepare_args(self, parser, plotData):
		super(Efficiency, self).prepare_args(parser, plotData)


	def run(self, plotData=None):
		super(Efficiency, self).run(plotData)

		if not len(plotData.plotdict['nicks']) >= 2:
			raise ValueError("At least two root histograms must be input.")
		eff = ROOT.TEfficiency(plotData.plotdict["root_objects"][plotData.plotdict['nicks'][0]], 
				plotData.plotdict["root_objects"][plotData.plotdict['nicks'][1]])
		graph = eff.CreateGraph()
		histogram_nick = "{0}_vs_{1}".format(plotData.plotdict['nicks'][0], plotData.plotdict['nicks'][1])
		plotData.plotdict['nicks'].append(histogram_nick)
		plotData.plotdict["root_objects"][histogram_nick] = graph

