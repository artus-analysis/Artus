# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class PrintBinContent(analysisbase.AnalysisBase):

	def __init__(self):
		super(PrintBinContent, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(PrintBinContent, self).modify_argument_parser(parser, args)
	
	def prepare_args(self, parser, plotData):
		super(PrintBinContent, self).prepare_args(parser, plotData)
	
	def run(self, plotData=None):
		super(PrintBinContent, self).run(plotData)
		
		for nick, root_object in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_object, ROOT.TH1):
				log.info("\n")
				log.info("{:*^50}".format(" Bin content of: {} ".format(nick)))
				log.info("{0:<9}| {1:<21}| {2:<21}".format("Bin Nr.",
				                                           "BinLowEdge",
				                                           "BinContent"))

				for iBin in range(0, root_object.GetNbinsX()+2):
					#underflow bin
					if iBin == 0:
						log.info("{0:<10} {1:<22} {2:<22}".format("underflow",
						                                          "---", root_object.GetBinContent(iBin)))

					#overflow bin
					elif iBin == root_object.GetNbinsX()+1:
						log.info("{0:<10} {1:<22} {2:<22}".format("overflow",
						                                          "---", root_object.GetBinContent(iBin)))

					else:
						log.info("{0:<10} {1:<22.1f} {2:<22.3f}".format(iBin,
						root_object.GetBinLowEdge(iBin), root_object.GetBinContent(iBin)))
				
