# -*- coding: utf-8 -*-

"""
"""
import os
import fnmatch
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

import Artus.Utility.jsonTools as json_tools

json_tools.JsonDict.COMMENT_DELIMITER = "@"


class HarryCore(object):

	def __init__(self, modules_dirs=None, args_from_script=None):
		super(HarryCore, self).__init__()
		
		# First time parsing of cmd arguments
		self.parser = harryparser.HarryParser()
		self._args_from_script = args_from_script.split() if args_from_script else None
		args, unknown_args = self.parser.parse_known_args(args_from_script)
		self._args = vars(args)

		# Default directories to be searched for plugins

		self._modules_dirs = ["input_modules/", "analysis_modules/", "plot_modules/"]
		if self._args['modules_search_paths']:
			self._modules_dirs += self._args['modules_search_paths']
		if modules_dirs is not None:
			self._modules_dirs += modules_dirs

		# Dict of all available processors
		self.available_processors = {}
		# List of active processors
		self.processors = []

		self._detect_available_processors()

	def _detect_available_processors(self):
		"""Detect all valid processors in modules_dirs and add them to avalaible processors."""
		# Get absolute paths to module directories
		modules_dirs = []
		# TODO: Needs to be extended for all usecases
		for module_dir in self._modules_dirs:
			# absolute path
			if os.path.isdir(module_dir):
				modules_dirs.append(module_dir)
			# relative path to current file
			if os.path.isdir(os.path.join(os.path.dirname(os.path.realpath(__file__)), module_dir)):
				modules_dirs.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), module_dir))
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
			except ImportError:
				log.debug("Failed to import module {0} from {1}.".format(module_name, filename))
				pass

	def run(self):
		""" Add all requested processors, then reparse all command line arguments.
		    Finally prepare and run all processors.
		"""
		json_default_initialisation = None
		if self._args["json_defaults"] != None:
			json_default_initialisation = self._args["json_defaults"]
			json_defaults = json_tools.JsonDict(self._args["json_defaults"]).doIncludes().doComments()
			#set_defaults will overwrite/ignore the json_default argument. Cannot be used.
			self._args.update(json_defaults)

		# replace 'json_defaults' from imported json file to actual name of imported json file
		if json_default_initialisation != None:
			self._args["json_defaults"] = json_default_initialisation

		if self._args["list_available_modules"]:
			self.print_available_modules()
			return
		
		# handle input modules (first)
		if self._args["input_module"] not in self.available_processors:
			log.warning("Input module \"" + self._args["input_module"] + "\" not found! Fall back to default \"%s\"!" % (self.parser.get_default("input_modules")))
			self._args["input_module"] = self.parser.get_default("input_module")
		self.processors.append(self.available_processors[self._args["input_module"]]())
		
		# handle analysis modules (second)
		if self._args["analysis_modules"] is None:
			self._args["analysis_modules"] = []
		
		available_modules = [module for module in self._args["analysis_modules"]
		                     if module in self.available_processors.keys() and
		                     issubclass(self.available_processors[module], AnalysisBase)]
		self.processors.extend([self.available_processors[module]() for module in available_modules])
		
		missing_modules = [module for module in self._args["analysis_modules"]
		                   if module not in self.available_processors.keys() or
		                   not issubclass(self.available_processors[module], AnalysisBase)]

		if len(missing_modules) > 0:
			log.warning("Some requested analysis modules have not been registered!")
			for module in missing_modules:
				log.warning("\t" + module)
		
		# handle plot modules (third)
		if isinstance(self._args["plot_modules"], basestring):
			self._args["plot_modules"] = [self._args["plot_modules"]]

		available_modules = [module for module in self._args["plot_modules"]
		                     if module in self.available_processors and
		                     issubclass(self.available_processors[module], PlotBase)]
		if len(available_modules) == 0:
			log.warning("No plot module found! Fall back to default \"%s\"!" % (self.parser.get_default("plot_modules")))
			available_modules = [self.parser.get_default("plot_modules")]
		self.processors.extend([self.available_processors[module]() for module in available_modules])
		
		missing_modules = [module for module in self._args["plot_modules"]
		                   if module not in self.available_processors or
		                   not issubclass(self.available_processors[module], PlotBase)]

		if len(missing_modules) > 0:
			log.warning("Some requested plot modules have not been registered!")
			for module in missing_modules:
				log.warning("\t" + module)
		
		# let processors modify the parser and then parse the arguments again
		for processor in self.processors:
			processor.modify_argument_parser(self.parser, self._args)
		
		# overwrite defaults by defaults from json files
		if self._args["json_defaults"] != None:
			self.parser.set_defaults(**(json_tools.JsonDict(self._args["json_defaults"]).doIncludes().doComments()))
		
		self._args = vars(self.parser.parse_args(self._args_from_script))
		plotData = plotdata.PlotData(self._args)
		
		# general ROOT settings
		ROOT.TH1.SetDefaultSumw2(True)
		ROOT.gROOT.SetBatch(True)
		
		# export arguments into JSON file
		# remove entries from dictionary that are not meant to be exported
		if self._args["export_json"] != None:
			save_args = dict(self._args)
			save_args.pop("quantities")
			save_args.pop("export_json")
			save_args.pop("live")
			save_args.pop("json_defaults")
			if self._args["export_json"] != "":
				save_name = self._args["export_json"]
			else:
				save_name = self._args["json_defaults"][0]

			if save_name != None:
				json_tools.JsonDict(save_args).save(save_name, indent=4)
			else: log.warning("No JSON could be exported. Please provide a filename.")
		
		# prepare aguments for all processors before running them
		for processor in self.processors:
			tmpPlotData = copy.deepcopy(plotData) if isinstance(processor, PlotBase) else plotData
			processor.prepare_args(self.parser, tmpPlotData)
			processor.run(tmpPlotData)
			if not isinstance(processor, PlotBase):
				plotData = tmpPlotData
		
		del(plotData)
	
	def register_processor(self, processor_name, processor):
		self.available_processors[processor_name] = processor


	def print_available_modules(self):
		""" Prints all available modules to stdout."""

		print "Valid input modules:"
		input_modules = sorted([module for module in self.available_processors if issubclass(self.available_processors[module], InputBase)])
		for module in input_modules:
			print "  {}".format(module)
			if inspect.getdoc(self.available_processors[module]):
				print "    ".format(inspect.getdoc(self.available_processors[module]))
		print ""
		print "Valid analysis modules:"
		analysis_modules = sorted([module for module in self.available_processors if issubclass(self.available_processors[module], AnalysisBase)])
		for module in analysis_modules:
			print "  {}".format(module)
			if inspect.getdoc(self.available_processors[module]):
				print "    {}".format(inspect.getdoc(self.available_processors[module]))
		print ""
		print "Valid Plot modules:"
		plot_modules = sorted([module for module in self.available_processors if issubclass(self.available_processors[module], PlotBase)])
		for module in plot_modules:
			print "  {}".format(module)
			if inspect.getdoc(self.available_processors[module]):
				print "    {}".format(inspect.getdoc(self.available_processors[module]))


