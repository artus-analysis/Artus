# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class PrintCPAPhi(analysisbase.AnalysisBase):

	def __init__(self):
		super(PrintCPAPhi, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(PrintCPAPhi, self).modify_argument_parser(parser, args)
	
	def prepare_args(self, parser, plotData):
		super(PrintCPAPhi, self).prepare_args(parser, plotData)
	
	def run(self, plotData=None):
		super(PrintCPAPhi, self).run(plotData)
		
		for nick, root_object in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_object, ROOT.TH1):
				log.info("\n")
				log.info("{:*^50}".format(" Bin content of: {} ".format(nick)))
				log.info("{0:<9}| {1:<21}| {2:<21}".format("Bin Nr.",
				                                           "BinLowEdge",
				                                           "BinContent"))
				bincontent=[]
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
						bincontent.append(root_object.GetBinContent(iBin))
				if root_object.GetNbinsX() == 3:
					aphi=(bincontent[0]-bincontent[1]+bincontent[2])/(bincontent[0]+bincontent[1]+bincontent[2])
					log.info("\n")
					log.info("{0:<10} {1:<22.3f}".format("APhi", aphi))
					log.info("\n")

				elif root_object.GetNbinsX() == 4:
					aphi=(bincontent[0]-bincontent[1]-bincontent[2]+bincontent[3])/(bincontent[0]+bincontent[1]+bincontent[2]+bincontent[3])
					log.info("\n")
					log.info("{0:<10} {1:<22.3f}".format("APhi", aphi))
					log.info("\n")
				
			elif isinstance(root_object, ROOT.TGraph):
				log.info("\n")
				log.info("{:*^50}".format(" Bin content of: {} ".format(nick)))
				log.info("{0:<9}| {1:<21}| {2:<21}".format("Bin Nr.",
				                                           "BinLowEdge",
				                                           "BinContent"))

				for iBin in range(0, root_object.GetN()):
					d1, d2 = ROOT.Double(0), ROOT.Double(0)
					root_object.GetPoint(iBin, d1, d2)
					log.info("{0:<10} {1:<22.3f} {2:<22.3f}".format(iBin,d1, d2))


