# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


class Processor(object):
	def __init__(self):
		"""
		Initialise this Processor.
		
		This class serves as a base class for all object that can run on the PlotData.
		"""
		pass
	
	def modify_argument_parser(self, parser):
		"""
		Option to modify the HarryParser.
		
		One can add new arguments as well as overwriting old ones and set new default values.
		"""
		pass
	
	def prepare_args(self, plotData):
		"""
		Function that checks the syntax of the arguments and prepares them to have the needed format.
		"""
		pass
	
	def run(self, plotData):
		"""
		Run this processor to perform its task.
		"""
		log.debug("Running processor " + self.__class__.__name__ + "...")
	
	@classmethod
	def name(cls):
		"""
		Returns a unique processor name.
		"""
		return cls.__name__

