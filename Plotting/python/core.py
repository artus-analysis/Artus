# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import HarryPlotter.Plotting.analysisbase as analysisbase
import HarryPlotter.Plotting.harryparser as harryparser
import HarryPlotter.Plotting.inputbase as inputbase
import HarryPlotter.Plotting.inputroot as inputroot
import HarryPlotter.Plotting.inputinteractive as inputinteractive
import HarryPlotter.Plotting.plotbase as plotbase
import HarryPlotter.Plotting.plotdata as plotdata
import HarryPlotter.Plotting.plotmpl as plotmpl
import HarryPlotter.Plotting.plotroot as plotroot
import HarryPlotter.Plotting.exportroot as exportroot
import HarryPlotter.Plotting.processor as processor

import HarryPlotter.Plotting.modules.eventselectionoverlap as eventselectionoverlap
import HarryPlotter.Plotting.modules.projectbyfit as projectbyfit
import HarryPlotter.Plotting.modules.functionplot as functionplot
import HarryPlotter.Plotting.modules.shapeyieldmerge as shapeyieldmerge
import HarryPlotter.Plotting.modules.extrapolationfactor as extrapolationfactor
import HarryPlotter.Plotting.modules.binerrorsofemptybins as binerrorsofemptybins
from HarryPlotter.Plotting.modules.normalization import NormalizeByBinWidth, NormalizeToUnity, NormalizeToFirstHisto

import HarryPlotter.Plotting.modules.correctnegativebincontents as correctnegativebincontents
import HarryPlotter.Plotting.modules.printinfos as printinfos

import HarryPlotter.Utility.jsonTools as json_tools
json_tools.JsonDict.COMMENT_DELIMITER = "@"


class HarryCore(object):
	def __init__(self, user_processors=None):
		super(HarryCore, self).__init__()
		
		if user_processors == None:
			user_processors = {}
		
		self.available_processors = {
			inputroot.InputRoot.name() : inputroot.InputRoot(),
			inputinteractive.InputInteractive.name() : inputinteractive.InputInteractive(),
			eventselectionoverlap.EventSelectionOverlap.name() : eventselectionoverlap.EventSelectionOverlap(),
			projectbyfit.ProjectByFit.name() : projectbyfit.ProjectByFit(),
			functionplot.FunctionPlot.name() : functionplot.FunctionPlot(),
			shapeyieldmerge.ShapeYieldMerge.name() : shapeyieldmerge.ShapeYieldMerge(),
			extrapolationfactor.ExtrapolationFactor.name() : extrapolationfactor.ExtrapolationFactor(),
			binerrorsofemptybins.BinErrorsOfEmptyBins.name() : binerrorsofemptybins.BinErrorsOfEmptyBins(),
			NormalizeByBinWidth.name(): NormalizeByBinWidth(),
			NormalizeToUnity.name(): NormalizeToUnity(),
			NormalizeToFirstHisto.name(): NormalizeToFirstHisto(),
			correctnegativebincontents.CorrectNegativeBinContents.name() : correctnegativebincontents.CorrectNegativeBinContents(),
			printinfos.PrintInfos.name() : printinfos.PrintInfos(),
			plotroot.PlotRoot.name() : plotroot.PlotRoot(),
			plotmpl.PlotMpl.name() : plotmpl.PlotMpl(),
			exportroot.ExportRoot.name() : exportroot.ExportRoot(),
		}
		self.available_processors.update(user_processors)
		self.processors = []
	
	def run(self, args_from_script=None):
		parser = harryparser.HarryParser()
		args, unknown_args = parser.parse_known_args(args_from_script.split() if args_from_script != None else None)
		args = vars(args)
		json_default_initialisation = None
		if args["json_defaults"] != None:
			json_default_initialisation = args["json_defaults"]
			json_defaults = json_tools.JsonDict(args["json_defaults"]).doIncludes().doComments()
			#set_defaults will overwrite/ignore the json_default argument. Cannot be used.
			args.update({k:v for k,v in json_defaults.iteritems() if (args.get(k) == None and v != None)})

		# replace 'json_defaults' from imported json file to actual name of imported json file
		if json_default_initialisation != None:
			args["json_defaults"] = json_default_initialisation
		
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
			parser.set_defaults(**(json_tools.JsonDict(args["json_defaults"]).doIncludes().doComments()))
		
		args = vars(parser.parse_args(args_from_script.split() if args_from_script != None else None))
		plotData = plotdata.PlotData(args)
		
		# general ROOT settings
		ROOT.TH1.SetDefaultSumw2(True)
		ROOT.gROOT.SetBatch(True)
		
		# export arguments into JSON file
		# remove entries from dictionary that are not meant to be exported
		if args["export_json"] != None:
			save_args = dict(args)
			save_args.pop("quantities")
			save_args.pop("export_json")
			save_args.pop("live")
			save_args.pop("json_defaults")	
			if args["export_json"] != "":
				save_name = args["export_json"]
			else:
				save_name = args["json_defaults"][0]

			if save_name != None:	
				json_tools.JsonDict(save_args).save(save_name, indent=4)
			else: log.warning("No JSON could be exported. Please provide a filename.")
		
		# prepare aguments for all processors before running them
		for processor in self.processors:
			processor.prepare_args(parser, plotData)
			processor.run(plotData)
	
	def register_processor(self, processor_name, processor):
		self.available_processors[processor_name] = processor

