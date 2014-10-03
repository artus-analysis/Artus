# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)
import sys
import ROOT
import hashlib
import HarryPlotter.Plotting.analysisbase as analysisbase
import HarryPlotter.Plotting.roottools as roottools

import HarryPlotter.Plotting.modules.functionplot as functionplot
import copy

class ProjectByFit(analysisbase.AnalysisBase):
	def __init__(self):
		super(ProjectByFit, self).__init__()

	def modify_argument_parser(self, parser, args):
		self.projectbyfit_options = parser.add_argument_group("Options for ProjectByFit module. See TH2::FitSlicesY for documentation.")
		self.projectbyfit_options.add_argument("--projection-function", type=str, nargs="+", default=None,
								help="Function to project along Y-Axis")
		self.projectbyfit_options.add_argument("--projection-parameters", type=str, nargs="+", default=None,
								help="Starting parameters for the fit, comma seperated.")
		self.projectbyfit_options.add_argument("--projection-to-plot", type=int, nargs="+", default=0,
								help="Parameter number that should be plotted.")
		self.projectbyfit_options.add_argument("--projection-fit-range", type=str, nargs="+", default=None,
								help="Y-Range to perform the fit in the format \"low,high\". Default is the whole range of the input 2D Histogram.")
		self.projectbyfit_options.add_argument("--projection-to-nick", type=str, nargs="+", default="nick0",
						help="Nickname of 2D input histogram. Default: nick0")

	def prepare_args(self, parser, plotData):
		super(ProjectByFit, self).prepare_args(parser, plotData)

		self.prepare_list_args(plotData, ["projection_function", "projection_parameters", "projection_to_plot","projection_fit_range","projection_to_nick"])

		plotData.plotdict["projection_fit_range"] = [ x.replace("\\", "") if x!=None else x for x in plotData.plotdict["projection_fit_range"] ]

	def run(self, plotData=None):
		super(ProjectByFit, self).run()

		histograms_to_replace = []
		result_histograms = {}
		for function, start_parameter, output_selection, fit_range, nick in zip(plotData.plotdict["projection_function"], 
		                                                                plotData.plotdict["projection_parameters"], 
		                                                                plotData.plotdict["projection_to_plot"],
		                                                                plotData.plotdict["projection_fit_range"],
		                                                                plotData.plotdict["projection_to_nick"]):

			histogram_2D = plotData.plotdict["root_objects"][nick]

			if not histogram_2D.ClassName().startswith("TH2"):
				log.fatal("The ProjectByFit Module only works on projecting TH2 histograms to TH1 histograms, but " + histogram_2D.ClassName() + " was provided as input.")
				sys.exit(1)

			if fit_range != None:
				fit_min = float(fit_range.split(",")[0])
				fit_max =  float(fit_range.split(",")[1])
			else: 
				fit_min = histogram_2D.GetYaxis().GetXmin()
				fit_max = histogram_2D.GetYaxis().GetXmax()
			start_parameters = [float(x) for x in start_parameter.split(",")]

			FunctionPlot = functionplot.FunctionPlot()
			root_function = FunctionPlot.create_function(function, fit_min, fit_max, start_parameters, nick=nick)

			histogram_2D.FitSlicesY(root_function)
			fitted_histogram = copy.deepcopy(ROOT.gDirectory.Get(histogram_2D.GetName() + "_" + str(output_selection) ))

			#remove original histogram and replace by projection
			result_histograms[nick] = fitted_histogram
			histograms_to_replace.append(nick)

		for nick in histograms_to_replace:
			del(plotData.plotdict["root_objects"][nick])
		plotData.plotdict["root_objects"].update(result_histograms)
