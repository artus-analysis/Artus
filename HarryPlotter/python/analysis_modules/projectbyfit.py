# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import sys
import ROOT
import hashlib
import Artus.HarryPlotter.analysisbase as analysisbase

import Artus.HarryPlotter.analysis_modules.functionplot as functionplot
import Artus.HarryPlotter.analysis_modules.projectionY as projectionY
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
		self.projectbyfit_options.add_argument("--projection-to-plot", type=str, nargs="+", default="0",
								help="Parameter number that should be plotted or TFormula to combine parameters of fit.")
		self.projectbyfit_options.add_argument("--projection-fit-range", type=str, nargs="+", default=None,
								help="Y-Range to perform the fit in the format \"low,high\". Default is the whole range of the input 2D Histogram.")
		self.projectbyfit_options.add_argument("--projection-to-nick", type=str, nargs="+", default="nick0",
						help="Nickname of 2D input histogram. Default: nick0")
		self.projectbyfit_options.add_argument("--fit-backend", type=str, nargs="+", default="ROOT",
								help="Fit backend. ROOT and RooFit are available. Check sourcecode which parts of RooFit are implemented. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(ProjectByFit, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["projection_function", "projection_parameters", "projection_to_plot","projection_fit_range","projection_to_nick", "fit_backend"])
		plotData.plotdict["projection_fit_range"] = [ x.replace("\\", "") if x != None else x for x in plotData.plotdict["projection_fit_range"] ]

	def run(self, plotData=None):
		super(ProjectByFit, self).run()

		histograms_to_replace = []
		result_histograms = {}
		for function, start_parameter, output_selection, fit_range, nick, fit_backend in zip(plotData.plotdict["projection_function"],
																							plotData.plotdict["projection_parameters"],
																							plotData.plotdict["projection_to_plot"],
																							plotData.plotdict["projection_fit_range"],
																							plotData.plotdict["projection_to_nick"],
																							plotData.plotdict["fit_backend"]):

			histogram_2D = plotData.plotdict["root_objects"][nick]

			if not histogram_2D.ClassName().startswith("TH2"):
				log.fatal("The ProjectByFit Module only works on projecting TH2 histograms to TH1 histograms, but " + histogram_2D.ClassName() + " was provided as input.")
				sys.exit(1)
			# Estimating fit range
			if fit_range != None:
				fit_min = float(fit_range.split(",")[0])
				fit_max = float(fit_range.split(",")[1])
			else:
				fit_min = histogram_2D.GetYaxis().GetXmin()
				fit_max = histogram_2D.GetYaxis().GetXmax()
			start_parameters = [float(x) for x in start_parameter.split(",")]
			# Getting fitted histogram
			if fit_backend == "ROOT":
				root_function = functionplot.FunctionPlot.create_tf1(function, fit_min, fit_max, start_parameters, nick=nick)
				histogram_2D.FitSlicesY(root_function)

				if output_selection.isdigit():
					fitted_histogram = copy.deepcopy(ROOT.gDirectory.Get(histogram_2D.GetName() + "_" + str(output_selection)))
				else:
					fitted_histograms = []
					for i in range(len(start_parameters)):
						fitted_histograms.append(copy.deepcopy(ROOT.gDirectory.Get(histogram_2D.GetName() + "_" + str(i))))
					fitted_histogram = self.postprocess_fits(fitted_histograms, output_selection)
			elif fit_backend == "RooFit":
				fitted_histogram = self.fitSlicesRooFit(histogram_2D, function, start_parameters, output_selection)
			else:
				log.fatal("No such backend")
				sys.exit(1)

			# TODO: this might be not so safe if one want to reuse the histogram
			# Remove original histogram and replace by projection
			result_histograms[nick] = fitted_histogram
			histograms_to_replace.append(nick)

		for nick in histograms_to_replace:
			del(plotData.plotdict["root_objects"][nick])

		plotData.plotdict["root_objects"].update(result_histograms)

	@staticmethod
	def fitSlicesRooFit(histogram_2D, function, start_parameters, output_selection):
		name = histogram_2D.GetName()
		result_histo = ROOT.TH1F(histogram_2D.GetName() + "_projection", "",
								 histogram_2D.GetNbinsX(),
								 histogram_2D.GetXaxis().GetXmin(),
								 histogram_2D.GetXaxis().GetXmax())

		slice_generator =  projectionY.ProjectionY.histoSlice(histogram_2D)
		for ibin, fit_histogram in enumerate(slice_generator):
			val, error = functionplot.FunctionPlot.get_roofit_parameter(function,
											  fit_histogram.GetXaxis().GetXmin(),
											  fit_histogram.GetXaxis().GetXmax(),
											  start_parameters,
											  fit_histogram,
											  output_selection)
			result_histo.SetBinContent(ibin + 1, val)
			result_histo.SetBinError(ibin + 1, error)

		return result_histo

	@staticmethod
	def postprocess_fits(fitted_histograms, equation):
		result_histo = ROOT.TH1F(fitted_histograms[0].GetName() + "_projection", "",
								 fitted_histograms[0].GetNbinsX(),
								 fitted_histograms[0].GetXaxis().GetXmin(),
								 fitted_histograms[0].GetXaxis().GetXmax())
		root_formula = ROOT.TFormula("postprocess_fits", equation)

		for ibin in xrange(1, fitted_histograms[0].GetNbinsX() + 1):
			for i, fitted_histogram in enumerate(fitted_histograms):
				root_formula.SetParameter(i, fitted_histogram.GetBinContent(ibin))
			result_histo.SetBinContent(ibin, root_formula.Eval(0))

		return result_histo
