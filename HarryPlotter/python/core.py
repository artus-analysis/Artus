# -*- coding: utf-8 -*-

"""
"""
import os
import fnmatch
import shlex
import sys
import imp
import inspect
import copy
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.HarryPlotter.harryparser as harryparser
import Artus.HarryPlotter.plotdata as plotdata

from Artus.HarryPlotter.analysisbase import AnalysisBase
from Artus.HarryPlotter.inputbase import InputBase
from Artus.HarryPlotter.plotbase import PlotBase

import Artus.HarryPlotter.processor as processor

from Artus.Utility.jsonTools import JsonDict
import Artus.Utility.tools as tools

JsonDict.COMMENT_DELIMITER = "@"


class HarryCore(object):

	def __init__(self, additional_modules_dirs=None, args_from_script=None, parser=None):
		super(HarryCore, self).__init__()

		# List of absolute paths to all module directories
		self._modules_dirs = []
		# Dict of all available processors
		self.available_processors = {}
		# List of active processors
		self.processors = []

		# First time parsing of cmd arguments
		if parser == None:
			parser = harryparser.HarryParser()
		self.parser = parser
		self._args_from_script = shlex.split(args_from_script) if args_from_script else None
		args, unknown_args = self.parser.parse_known_args(self._args_from_script)
		self.args = vars(args)

		# Default directories to be searched for plugins
		default_modules_dirs = ["input_modules/", "analysis_modules/", "plot_modules/"]

		# Modules search dir from command line arguments
		if self.args['modules_search_paths']:
			default_modules_dirs += self.args['modules_search_paths']
		# Passed additonal modules dirs
		if additional_modules_dirs:
			default_modules_dirs += additional_modules_dirs
		for directory in default_modules_dirs:
			self.register_modules_dir(directory)

	def _detect_available_processors(self):
		"""Detect all valid processors in modules_dirs and add them to avalaible processors."""

		modules_dirs = self._modules_dirs
		# Loop over all possible module files
		matches = []
		for module_dir in modules_dirs:
			for root, dirnames, filenames in os.walk(module_dir):
				for filename in fnmatch.filter(filenames, '*.py'):
					matches.append(os.path.join(root, filename))

		for filename in matches:
			try:
				log.debug("Try to import module from path {0}.".format(filename))
				module_name = os.path.splitext(os.path.basename(filename))[0]
				module = imp.load_source(module_name, filename)
				for name, obj in inspect.getmembers(module):
					if inspect.isclass(obj):
						if (issubclass(obj, AnalysisBase) or issubclass(obj, InputBase) or
						    issubclass(obj, PlotBase)):
							log.debug("Adding module {0} to available processors.".format(obj.name()))
							self.available_processors[obj.name()] = obj
			except ImportError as e:
				log.debug("Failed to import module {0} from {1}.".format(module_name, filename))
				log.debug("Error message {0}.".format(e))
				pass

	def run(self):
		"""Add all requested processors, then reparse all command line arguments.
		   Finally prepare and run all processors.
		"""
		# Detect all valid processors
		self._detect_available_processors()

		json_default_initialisation = None
		if self.args["json_defaults"] is not None:
			json_default_initialisation = self.args["json_defaults"]
			json_defaults = JsonDict(self.args["json_defaults"]).doIncludes().doComments()
			#set_defaults will overwrite/ignore the json_default argument. Cannot be used.
			no_default_args = dict((k,v) for (k,v) in self.args.items() if not self.parser.get_default(k) == self.args[k] )
			self.args.update(dict(json_defaults.items() + no_default_args.items()))

		# replace 'json_defaults' from imported json file to actual name of imported json file
		if json_default_initialisation != None:
			self.args["json_defaults"] = json_default_initialisation

		if self.args["list_available_modules"]:
			self._print_available_modules()
			return
		
		# handle input modules (first)
		if self._isvalid_processor(self.args["input_module"], processor_type=InputBase):
			self.processors.append(self.available_processors[self.args["input_module"]]())
		else:
			log.info("Provide a valid input module or none at all. Default is \"{0}\"!".format(self.parser.get_default("input_modules")))
			log.critical("Input module \"{0}\" not found!".format(self.args["input_module"]))

		# handle analysis modules (second)
		if self.args["analysis_modules"] is None:
			self.args["analysis_modules"] = []
		
		for module in self.args["analysis_modules"]:
			if self._isvalid_processor(module, processor_type=AnalysisBase):
				self.processors.append(self.available_processors[module]())
			else:
				log.critical("Analysis module \"{0}\" not found!".format(module))

		# handle plot modules (third)
		if isinstance(self.args["plot_modules"], basestring):
			self.args["plot_modules"] = [self.args["plot_modules"]]
		if not self.args["plot_modules"]:
			log.critical("Empty list of plot modules supplied!")

		for module in self.args["plot_modules"]:
			if self._isvalid_processor(module, processor_type=PlotBase):
				self.processors.append(self.available_processors[module]())
			else:
				log.critical("Plot module \"{0}\" not found!".format(module))

		# let processors modify the parser and then parse the arguments again
		for processor in self.processors:
			processor.modify_argument_parser(self.parser, self.args)
		
		# overwrite defaults by defaults from json files
		if self.args["json_defaults"] != None:
			self.parser.set_defaults(**(JsonDict(self.args["json_defaults"]).doIncludes().doComments()))
		
		self.args = vars(self.parser.parse_args(self._args_from_script))
		plotData = plotdata.PlotData(self.args)
		
		# print the final processor chain
		log.debug('Processors will be run in the following order')
		log.debug(' => '.join([processor.name() for processor in self.processors]))

		if not plotData.plotdict['no_logo']:
			self._logo()

		# general ROOT settings
		log.debug("Setting ROOT TH1 DefaultSumw2 to True.")
		ROOT.TH1.SetDefaultSumw2(True)
		ROOT.gROOT.SetBatch(True)
		
		# export arguments into JSON file (1)
		# remove entries from dictionary that are not meant to be exported
		export_args = JsonDict(copy.deepcopy(plotData.plotdict))
		for arg in ["quantities", "export_json", "live", "userpc", "json_defaults"]:
			export_args.pop(arg, None)
		# remove defaults
		for key in export_args.keys():
			if (key in self.args and self.parser.get_default(key) == export_args[key]
						and (self.args.get("json_defaults", None) is None or
						key not in JsonDict(self.args["json_defaults"]).doIncludes().doComments())):
				export_args.pop(key, None)

		if plotData.plotdict["export_json"] == "update":
			plotData.plotdict["export_json"] = "default" if plotData.plotdict["json_defaults"] is None else plotData.plotdict["json_defaults"][0]
		
		# prepare aguments for all processors before running them
		for processor in self.processors:
			processor.prepare_args(self.parser, plotData)
			processor.run(plotData)
		
		# save plots
		output_filenames = plotData.save()
		
		# export arguments into JSON file (2)
		if plotData.plotdict["export_json"] != "default":
			export_args.save(plotData.plotdict["export_json"], indent=4)
		
		del(plotData)
		return output_filenames
	
	def register_processor(self, processor):
		"""Add processor to list of available processors."""
		if (issubclass(obj, AnalysisBase) or issubclass(obj, InputBase) or
		    issubclass(obj, PlotBase)):
			self.available_processors[processor.name()] = processor
		else:
			raise TypeError("Provided processor is of invalid type.")

	def register_modules_dir(self, module_dir):
		"""Add directory to list of searched directories for modules."""
		# Expand environment variables
		module_dir = os.path.expandvars(module_dir)
		# absolute path
		if os.path.isdir(module_dir):
			self._modules_dirs.append(module_dir)
		# relative path to current file
		elif os.path.isdir(os.path.join(os.path.dirname(os.path.realpath(__file__)), module_dir)):
			self._modules_dirs.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), module_dir))
		else:
			log.critical("Couldnt append %s to list of module directories!" % module_dir)

	def _isvalid_processor(self, processor_name, processor_type=None):
		"""Check if a processor is valid."""
		if not processor_name in self.available_processors:
			return False
		elif not (issubclass(self.available_processors[processor_name], AnalysisBase) or 
			      issubclass(self.available_processors[processor_name], InputBase) or
			      issubclass(self.available_processors[processor_name], PlotBase)):
			return False
		elif processor_type is not None and not issubclass(self.available_processors[processor_name], processor_type):
			return False
		else:
			return True

	def _print_available_modules(self):
		"""Prints all available modules to stdout."""
		title_strings = ["Input modules:", "\nAnalysis modules:", "\nPlot modules:"]
		baseclasses = [InputBase, AnalysisBase, PlotBase]
		for title_string, baseclass in zip(title_strings, baseclasses):
			log.info(tools.get_colored_string(title_string, "yellow"))
			self._print_module_list(sorted([module for module in self.available_processors if issubclass(self.available_processors[module], baseclass)]))

	def _print_module_list(self, module_list):
		"""Print a list of modules (name and docstring)"""
		for module in module_list:
			log.info(tools.get_colored_string("\t{}".format(module), "green"))
			if inspect.getdoc(self.available_processors[module]):
				log.info(tools.get_indented_text("\t\t", inspect.getdoc(self.available_processors[module])))


	def _logo(self):
		log.info("            _              _   ,                        ")
		log.info("       /b_,dM\__,_        ( |_|                         ")
		log.info("     _/MMMMMMMMMMMm,        | |_)arry                   ")
		log.info("    _YMMMMMMMMMMMM(          ___                        ")
		log.info("   `MMMMMM/   /   \         '_|_)                       ")
		log.info("    MMM|  __  / __/           | lotter                  ")
		log.info("    YMM/_/# \__/# \                      ______________ ")
		log.info("    (.   \__/  \__/                     |         |data|")
		log.info("      )       _,  | 'EXPLOTIARMUS!'     |       s |MC  |")
		log.info(" _____/\     _   /                   *  |       d '----|")
		log.info("     \  `._____,'                  *    |       d:     |")
		log.info("      `..___(__             * * * *  *  |      yNh     |")
		log.info("               ``-.        /    * *   * |      yMh     |")
		log.info("                   \      /      *  *   |     :mMm/    |")
		log.info("                         /3           * |__.-omMMMm+-__|\n")
