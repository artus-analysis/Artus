# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import collections


class Processor(object):
	def __init__(self):
		"""
		Initialise this Processor.
		
		This class serves as a base class for all object that can run on the PlotData.
		"""
		super(Processor, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		"""
		Option to modify the HarryParser.
		
		One can add new arguments as well as overwriting old ones and set new default values.
		"""
		pass
	
	def prepare_args(self, parser, plotData):
		"""
		Function that checks the syntax of the arguments and prepares them to have the needed format.
		"""
		pass
	
	def run(self, plotData):
		"""
		Run this processor to perform its task.
		"""
		log.debug("Running processor " + self.__class__.__name__ + "...")
	
	def prepare_list_args(self, plotData, keys_of_list_args, n_items=None):
		"""
		Prepare list-type entries in plotData. All values for given keys are casted into lists
		and then the lists are filled up by repeating existing items until all lists have the same size.
		"""
		# prepare lists
		for key in keys_of_list_args:
			if not isinstance(plotData.plotdict[key], collections.Iterable) or isinstance(plotData.plotdict[key], basestring):
				plotData.plotdict[key] = [plotData.plotdict[key]]
		
		# expand/cut lists that are too short/long
		max_n_inputs = n_items if n_items != None else max([len(plotData.plotdict[key]) for key in keys_of_list_args])
		for key, plot_list in [(key, plotData.plotdict[key]) for key in keys_of_list_args]:
			if len(plot_list) != 1 and len(plot_list) < max_n_inputs:
				log.warning("Argument '%s' with length %.0f will be repeated until length %.0f! Is this correct?" % (key, len(plot_list), max_n_inputs))
			plotData.plotdict[key] = (plot_list * max_n_inputs)[:max_n_inputs]
	
	@classmethod
	def name(cls):
		"""
		Returns a unique processor name.
		"""
		return cls.__name__

