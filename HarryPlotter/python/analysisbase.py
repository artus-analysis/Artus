# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import sys

import Artus.HarryPlotter.processor as processor


class AnalysisBase(processor.Processor):
	def __init__(self):
		super(AnalysisBase, self).__init__()
	
	def prepare_args(self, parser, plotData):
		super(AnalysisBase, self).prepare_args(parser, plotData)
	
	def run(self, plotData=None):
		super(AnalysisBase, self).run(plotData)

	def auto_set_arguments(self, plotData, arguments, result_argument=None, result_name=None):
		""" This function sets the arguments for a module if the number of 
			input arguments equals the number of nicks.
			Optionally, an argument for a result nick can be set.
			See e.g the Divide module to see how this function is called.
		"""

		if all(plotData.plotdict[i] == [None] for i in arguments):
			if len(plotData.plotdict['nicks']) == len(arguments):
				string = "{} module: Using ".format(self.__class__.__name__)
				string += ", ".join(["nick '{0}' for {1}".format(nick, key) for nick, key in zip(plotData.plotdict['nicks'], arguments)])
				if result_name is not None:
					string += ". New nick: '{}'".format(result_name)
				log.info(string)

				for i, arg in enumerate(arguments):
					plotData.plotdict[arg] = [plotData.plotdict['nicks'][i]]
				if result_argument is not None and result_name is not None:
					plotData.plotdict[result_argument] = [result_name]
			else:
				log.critical("No arguments for analysis module {}!".format(self.__class__.__name__))
				sys.exit(1)

