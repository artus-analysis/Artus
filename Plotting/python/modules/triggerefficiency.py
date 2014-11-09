# -*- coding: utf-8 -*-


import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase


class TriggerEfficiency(analysisbase.AnalysisBase):

	def __init__(self):
		super(TriggerEfficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(TriggerEfficiency, self).modify_argument_parser(parser, args)
		
		self.empty_bins_options = parser.add_argument_group("Trigger efficiency options")
		self.empty_bins_options.add_argument("--trigger-paths", nargs='*',
				help="List of two paths")

	def prepare_args(self, parser, plotData):
		super(TriggerEfficiency, self).prepare_args(parser, plotData)


	def run(self, plotData=None):
		super(TriggerEfficiency, self).run(plotData)

		if not len(plotData.plotdict['nicks']) >= 2:
			raise ValueError("At least two root histograms must be input.")
		eff = ROOT.TEfficiency(plotData.plotdict["root_objects"][plotData.plotdict['nicks'][0]], 
				plotData.plotdict["root_objects"][plotData.plotdict['nicks'][1]])
		graph = eff.CreateGraph()
		histogram_nick = "{0}_vs_{1}".format(plotData.plotdict['nicks'][0], plotData.plotdict['nicks'][1])
		plotData.plotdict['nicks'].append(histogram_nick)
		plotData.plotdict["root_objects"][histogram_nick] = graph

