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
	
	def prepare_list_args(self, plotData, keys_of_list_args, n_items=None, help=""):
		"""
		Prepare list-type entries in plotData. All values for given keys are casted into lists
		and then the lists are filled up by repeating existing items until all lists have the same size.
		"""
		# prepare lists
		for key in keys_of_list_args:
			if not isinstance(plotData.plotdict[key], collections.Iterable) or isinstance(plotData.plotdict[key], basestring):
				plotData.plotdict[key] = [plotData.plotdict[key]]

		# change "None" in input to None
		for key in keys_of_list_args:
			for index, item in enumerate(plotData.plotdict[key]):
				if(item == "None"):
					plotData.plotdict[key][index] = None

		max_n_inputs = n_items if n_items != None else max([len(plotData.plotdict[key]) for key in keys_of_list_args])
		
		# warn if any input requires replication to match length
		if any((1 < len(plotData.plotdict[key]) < max_n_inputs) for key in keys_of_list_args):
			(log.debug if plotData.plotdict["dry_run"] else log.warning)(
				"Parameters '%s' require parameter list length of %d."
				" Parameters %s will be replicated to match required length." % (
					"', '".join(key for key in keys_of_list_args if len(plotData.plotdict[key]) == max_n_inputs),
					max_n_inputs,
					", ".join("'%s'(%d)" % (key, len(plotData.plotdict[key])) for key in keys_of_list_args if 1 < len(plotData.plotdict[key]) < max_n_inputs),
				)
			)

		# expand/cut lists that are too short/long
		for key, plot_list in [(key, plotData.plotdict[key]) for key in keys_of_list_args]:
			plotData.plotdict[key] = (plot_list * max_n_inputs)[:max_n_inputs]
		
		if log.isEnabledFor(logging.DEBUG):
			log.debug("Argument list expansion: " + help)
			for index in xrange(len(plotData.plotdict[keys_of_list_args[0]])):
				log.debug("\tItem %d:" % index)
				for key, value in [(key, plotData.plotdict[key][index]) for key in keys_of_list_args]:
					log.debug("\t\t" + key + " -> " + str(value))
			log.debug("")

	def auto_set_arguments(self, plotData, arguments, result_argument=None, result_name=None):
		""" This function sets the arguments for a module if the number of 
			input arguments equals the number of nicks present in the plotdict.
			Optionally, an argument for a result nick can be set.

			See e.g the Divide module to see how this function is called. Normally
			this module would need 3 arguments, two of them being 'input' arguments
			(nick for denominator and numerator). If you have exactly 2 nicks in
			the plotdict, the auto_set_arguments function will set these two
			nicks as the two input arguments, and also set the third argument
			(result-nick) if not manually specified.
			This means that (in this case of two nicks present) the module can be
			used without manually specifying any arguments.
		"""

		if all(plotData.plotdict[i] == [None] for i in arguments):
			if len(plotData.plotdict['nicks']) == len(arguments):
				string = "{} module: Using ".format(self.__class__.__name__)
				string += ", ".join(["nick '{0}' for {1}".format(nick, key) for nick, key in zip(plotData.plotdict['nicks'], arguments)])

				for i, arg in enumerate(arguments):
					plotData.plotdict[arg] = [plotData.plotdict['nicks'][i]]
				if result_argument is not None:
					if plotData.plotdict[result_argument] != [None] and len(plotData.plotdict[result_argument])==1:
						result_name = plotData.plotdict[result_argument][0]
					if result_name is not None:
						plotData.plotdict[result_argument] = [result_name]
						string += ". New nick: '{}'".format(result_name)
				log.info(string)
			else:
				log.critical("No arguments for analysis module {}!".format(self.__class__.__name__))
				sys.exit(1)
	
	@classmethod
	def name(cls):
		"""
		Returns a unique processor name.
		"""
		return cls.__name__

