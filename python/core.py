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


class HarryCore(object):
	def __init__(self, user_processors=None):
		if user_processors == None:
			user_processors = {}
		
		self.available_processors = {
			inputroot.InputRoot.name() : inputroot.InputRoot(),
			plotroot.PlotRoot.name() : plotroot.PlotRoot(),
			plotmpl.PlotMpl.name() : plotmpl.PlotMpl(),
		}
		self.available_processors.update(user_processors)
		self.processors = []
	
	def run(self):
		parser = harryparser.HarryParser()
		args, unknown_args = parser.parse_known_args()
		
		self.processors = []
		
		# handle input modules (first)
		if args.input_module not in self.available_processors.keys():
			log.warning("Input module \"" + args.input_module + "\" not found! Fall back to default!")
			args.input_module = parser.get_default("input-module")
		self.processors.append(self.available_processors[args.input_module])
		
		# handle analysis modules (second)
		if args.analysis_modules == None:
			args.analysis_modules = []
		
		available_modules = [module for module in args.analysis_modules
		                     if module in self.available_processors.keys() and
		                     isinstance(self.available_processors[module], analysisbase.AnalysisBase)]
		self.processors.extend([self.available_processors[module] for module in available_modules])
		
		missing_modules = [module for module in args.analysis_modules
		                   if module not in self.available_processors.keys() or
		                   not isinstance(self.available_processors[module], analysisbase.AnalysisBase)]
		if len(missing_modules) > 0:
			log.warning("Some requested analysis modules have not been registered!")
			for module in missing_module:
				log.warning("\t" + module)
		
		# handle plot modules (third)
		if isinstance(args.plot_modules, basestring):
			args.plot_modules = [args.plot_modules]
		available_modules = [module for module in args.plot_modules
		                     if module in self.available_processors.keys() and
		                     isinstance(self.available_processors[module], plotbase.PlotBase)]
		if len(available_modules) == 0:
			log.warning("No registered plot module specified! Fall back to default!")
			available_modules = [parser.get_default("plot-modules")]
		self.processors.extend([self.available_processors[module] for module in available_modules])
		
		missing_modules = [module for module in args.plot_modules
		                   if module not in self.available_processors.keys() or
		                   not isinstance(self.available_processors[module], plotbase.PlotBase)]
		if len(missing_modules) > 0:
			log.warning("Some requested plot modules have not been registered!")
			for module in missing_module:
				log.warning("\t" + module)
		
		# let processors modify the parser and then parse the arguments again
		for processor in self.processors:
			processor.modify_argument_parser(parser)
		args = parser.parse_args()
		
		# general ROOT settings
		ROOT.TH1.SetDefaultSumw2(True)
		ROOT.gROOT.SetBatch(True)
		
		# run all processors
		plotData = plotdata.PlotData()
		
		for processor in self.processors:
			processor.run(plotData)
	
	def register_processor(processor_name, processor):
		self.available_processors[processor_name] = processor

