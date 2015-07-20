# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class PrintInfos(analysisbase.AnalysisBase):

	def __init__(self):
		super(PrintInfos, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(PrintInfos, self).modify_argument_parser(parser, args)
	
	def prepare_args(self, parser, plotData):
		super(PrintInfos, self).prepare_args(parser, plotData)
	
	def run(self, plotData=None):
		super(PrintInfos, self).run(plotData)
		
		for nick, root_object in plotData.plotdict["root_objects"].iteritems():
			log.info("\nInfos for object with nick \"{nick}\":".format(nick=nick))
			if log.isEnabledFor(logging.DEBUG):
				root_object.Print("range")
			else:
				root_object.Print()

