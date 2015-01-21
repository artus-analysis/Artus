# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class Efficiency(analysisbase.AnalysisBase):
	"""Build efficiency graphs from two histograms"""

	def __init__(self):
		super(Efficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Efficiency, self).modify_argument_parser(parser, args)
		
		self.efficiency_options = parser.add_argument_group("efficiency factor options")
		self.efficiency_options.add_argument("--efficiency-numerator-nicks", nargs="+", required=True,
				help="Nick names for the numerators of the efficiencies.")
		self.efficiency_options.add_argument("--efficiency-denominator-nicks", nargs="+", required=True,
				help="Nick names for the denominator of the efficiencies.")
		self.efficiency_options.add_argument("--efficiency-nicks", nargs="+", default=[None],
				help="Nick names for the resulting efficiency graphs.")
		self.efficiency_options.add_argument("--efficiency-methods", nargs="+", default=["cp"],
				help="(Error) calculation methods. See TGraphAsymmErrors.Divide for further information. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(Efficiency, self).prepare_args(parser, plotData)
		
		plotData.plotdict["efficiency_numerator_nicks"] = [nick for nick in plotData.plotdict["efficiency_numerator_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["efficiency_denominator_nicks"] = [nick for nick in plotData.plotdict["efficiency_denominator_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["efficiency_nicks"] = plotData.plotdict["efficiency_nicks"]
		self.prepare_list_args(plotData, ["efficiency_numerator_nicks", "efficiency_denominator_nicks", "efficiency_nicks", "efficiency_methods"])
		
		for index, nick in enumerate(plotData.plotdict["efficiency_nicks"]):
			if not nick:
				nick = plotData.plotdict["efficiency_numerator_nicks"][index] + "_" + plotData.plotdict["efficiency_denominator_nicks"][index]
				plotData.plotdict["efficiency_nicks"][index] = nick
			if not nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(plotData.plotdict["efficiency_denominator_nicks"][index])+1, nick)

	def run(self, plotData=None):
		super(Efficiency, self).run(plotData)
		
		for index, (efficiency_numerator_nick, efficiency_denominator_nick, efficiency_nick, efficiency_method) in enumerate(zip(
				plotData.plotdict["efficiency_numerator_nicks"],
				plotData.plotdict["efficiency_denominator_nicks"],
				plotData.plotdict["efficiency_nicks"],
				plotData.plotdict["efficiency_methods"]
		)):
			
			efficiency_graph_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_objects"][efficiency_numerator_nick].GetName(),
			                                                             plotData.plotdict["root_objects"][efficiency_denominator_nick].GetName()])).hexdigest()
			
			efficiency_graph = ROOT.TGraphAsymmErrors(
					plotData.plotdict["root_objects"][efficiency_numerator_nick],
					plotData.plotdict["root_objects"][efficiency_denominator_nick],
					efficiency_method
			)
			efficiency_graph.SetName(efficiency_graph_name)
			efficiency_graph.SetTitle("")
			plotData.plotdict["root_objects"][efficiency_nick] = efficiency_graph

