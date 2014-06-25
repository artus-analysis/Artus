# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.harryparser as harryparser
import Artus.HarryPlotter.inputbase as inputbase
import Artus.HarryPlotter.inputroot as inputroot
import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata
import Artus.HarryPlotter.plotmpl as plotmpl
import Artus.HarryPlotter.plotroot as plotroot
import Artus.HarryPlotter.processor as processor

import Artus.HarryPlotter.modules.eventselectionoverlap as eventselectionoverlap

import Artus.Configuration.jsonTools as json_tools


class HarryCore(object):
	def __init__(self, user_processors=None):
		super(HarryCore, self).__init__()
		
		if user_processors == None:
			user_processors = {}
		
		self.available_processors = {
			inputroot.InputRoot.name() : inputroot.InputRoot(),
			eventselectionoverlap.EventSelectionOverlap.name() : eventselectionoverlap.EventSelectionOverlap(),
			plotroot.PlotRoot.name() : plotroot.PlotRoot(),
			plotmpl.PlotMpl.name() : plotmpl.PlotMpl(),
		}
		self.available_processors.update(user_processors)
		self.processors = []
	
	def run(self, args_from_script=None):
		parser = harryparser.HarryParser()
		args, unknown_args = parser.parse_known_args(args_from_script.split() if args_from_script != None else None)
		args = vars(args)
		
		if args["json_defaults"] != None:
			args.update(json_tools.JsonDict(args["json_defaults"]))
		
		self.processors = []
		
		# handle input modules (first)
		if args["input_module"] not in self.available_processors.keys():
			log.warning("Input module \"" + args["input_module"] + "\" not found! Fall back to default \"%s\"!" % (parser.get_default("input_modules")))
			args["input_module"] = parser.get_default("input_module")
		self.processors.append(self.available_processors[args["input_module"]])
		
		# handle analysis modules (second)
		if args["analysis_modules"] == None:
			args["analysis_modules"] = []
		
		available_modules = [module for module in args["analysis_modules"]
		                     if module in self.available_processors.keys() and
		                     isinstance(self.available_processors[module], analysisbase.AnalysisBase)]
		self.processors.extend([self.available_processors[module] for module in available_modules])
		
		missing_modules = [module for module in args["analysis_modules"]
		                   if module not in self.available_processors.keys() or
		                   not isinstance(self.available_processors[module], analysisbase.AnalysisBase)]
		if len(missing_modules) > 0:
			log.warning("Some requested analysis modules have not been registered!")
			for module in missing_modules:
				log.warning("\t" + module)
		
		# handle plot modules (third)
		if isinstance(args["plot_modules"], basestring):
			args["plot_modules"] = [args["plot_modules"]]
		available_modules = [module for module in args["plot_modules"]
		                     if module in self.available_processors.keys() and
		                     isinstance(self.available_processors[module], plotbase.PlotBase)]
		if len(available_modules) == 0:
			log.warning("No plot module found! Fall back to default \"%s\"!" % (parser.get_default("plot_modules")))
			available_modules = [parser.get_default("plot_modules")]
		self.processors.extend([self.available_processors[module] for module in available_modules])
		
		missing_modules = [module for module in args["plot_modules"]
		                   if module not in self.available_processors.keys() or
		                   not isinstance(self.available_processors[module], plotbase.PlotBase)]
		if len(missing_modules) > 0:
			log.warning("Some requested plot modules have not been registered!")
			for module in missing_modules:
				log.warning("\t" + module)
		
		# let processors modify the parser and then parse the arguments again
		for processor in self.processors:
			processor.modify_argument_parser(parser, args)
		
		# overwrite defaults by defaults from json files
		if args["json_defaults"] != None:
			parser.set_defaults(**(json_tools.JsonDict(args["json_defaults"])))
		
		args = vars(parser.parse_args(args_from_script.split() if args_from_script != None else None))
		plotData = plotdata.PlotData(args)
		
		# general ROOT settings
		ROOT.TH1.SetDefaultSumw2(True)
		ROOT.gROOT.SetBatch(True)
		
		# export arguments into JSON file
		if args["export_json"] != None:
			json_tools.JsonDict(args).save(args.pop("export_json"), indent=4)
		
		# prepare aguments for all processors before running them
		for processor in self.processors:
			processor.prepare_args(parser, plotData)
			processor.run(plotData)
	
	def register_processor(self, processor_name, processor):
		self.available_processors[processor_name] = processor

