# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

from math import sqrt, pow

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.analysis_modules.scaleerrors as scaleerrors


class Ratio(analysisbase.AnalysisBase):
	"""Add ratio of histograms"""

	def modify_argument_parser(self, parser, args):
		super(Ratio, self).modify_argument_parser(parser, args)

		self.ratio_options = parser.add_argument_group("{} options".format(self.name()))
		self.ratio_options.add_argument("--ratio-numerator-nicks", nargs="+",
				help="Nick names for the numerators of the ratio. Multiple nicks (whitespace separated) will be summed up before calculating the ratio.")
		self.ratio_options.add_argument("--ratio-numerator-no-errors", nargs="+", type="bool", default=[False],
				help="Remove errors of numerator histograms before calculating the ratio. [Default: %(default)s]")
		self.ratio_options.add_argument("--ratio-denominator-nicks", nargs="+",
				help="Nick names for the denominator of the ratio. Multiple nicks (whitespace separated) will be summed up before calculating the ratio.")
		self.ratio_options.add_argument("--ratio-denominator-no-errors", nargs="+", type="bool", default=[True],
				help="Remove errors of denominator histograms before calculating the ratio. [Default: %(default)s]")
		self.ratio_options.add_argument("--ratio-result-nicks", nargs="+",
				help="Nick names for the resulting ratio graphs.")
		self.ratio_options.add_argument("--ratio-compatibility-test-methods", nargs="+", default=[],
				choices=["AndersonDarlingTest", "Chi2Test", "KolmogorovTest"],
				help="Perform compatibility tests with test functions of ROOT.TH1. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(Ratio, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["ratio_numerator_nicks", "ratio_numerator_no_errors", "ratio_denominator_nicks", "ratio_denominator_no_errors", "ratio_result_nicks"])
		self.auto_set_arguments(plotData, ["ratio_numerator_nicks", "ratio_denominator_nicks"], "ratio_result_nicks", "ratio")

		for index, (ratio_numerator_nick, ratio_denominator_nick, ratio_result_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["ratio_numerator_nicks", "ratio_denominator_nicks", "ratio_result_nicks"]])):
			plotData.plotdict["ratio_numerator_nicks"][index] = ratio_numerator_nick.split()
			plotData.plotdict["ratio_denominator_nicks"][index] = ratio_denominator_nick.split()

			if ratio_result_nick is None:
				plotData.plotdict["ratio_result_nicks"][index] = "ratio_{num}_over_{denom}".format(
						num="_".join(plotData.plotdict["ratio_numerator_nicks"][index]),
						denom="_".join(plotData.plotdict["ratio_denominator_nicks"][index]))

			if not plotData.plotdict["ratio_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].append(plotData.plotdict["ratio_result_nicks"][index])

	@staticmethod
	def get_histograms(plotdict={}, ratio_nicks=None, ratio_numerator_nicks=None, ratio_denominator_nicks=None, ratio_result_nick=None):
		histogram = None

		for nick in ratio_nicks:

			root_object = plotdict[nick]
			if histogram is None:
				histogram = root_object.Clone(
						"ratio_" + hashlib.md5("_".join([str(ratio_nicks), str(ratio_denominator_nicks), ratio_result_nick])).hexdigest()
				)
			else: histogram.Add(root_object)

			if hasattr(histogram, "SetDirectory"): histogram.SetDirectory(0)

		return histogram

	def run(self, plotData=None):
		super(Ratio, self).run(plotData)

		for ratio_numerator_nicks, ratio_numerator_no_errors, ratio_denominator_nicks, ratio_denominator_no_errors, ratio_result_nick in zip(
				*[plotData.plotdict[k] for k in ["ratio_numerator_nicks", "ratio_numerator_no_errors", "ratio_denominator_nicks", "ratio_denominator_no_errors", "ratio_result_nicks"]]):

			# Create nick sum histograms
			numerator_histogram = self.get_histograms(plotdict=plotData.plotdict["root_objects"], ratio_nicks=ratio_numerator_nicks, ratio_numerator_nicks=ratio_numerator_nicks, ratio_denominator_nicks=ratio_denominator_nicks, ratio_result_nick=ratio_result_nick)
			denominator_histogram = self.get_histograms(plotdict=plotData.plotdict["root_objects"], ratio_nicks=ratio_denominator_nicks, ratio_numerator_nicks=ratio_numerator_nicks, ratio_denominator_nicks=ratio_denominator_nicks, ratio_result_nick=ratio_result_nick)
			
			# run compatibility tests if configured
			for test_method in plotData.plotdict["ratio_compatibility_test_methods"]:
				test_result = getattr(numerator_histogram, test_method)(denominator_histogram)
				log.info("{test_method} ( {ratio_numerator_nicks}, {ratio_denominator_nicks} ) = {test_result}".format(
						test_method=test_method,
						ratio_numerator_nicks="+".join(ratio_numerator_nicks),
						ratio_denominator_nicks="+".join(ratio_denominator_nicks),
						test_result=str(test_result)
				))

			if ratio_numerator_no_errors: scaleerrors.ScaleErrors.scale_errors(numerator_histogram, scale_factor=0.0)
			if ratio_denominator_no_errors: scaleerrors.ScaleErrors.scale_errors(denominator_histogram, scale_factor=0.0)
			
			# preparations for divisions with one function
			if ((isinstance(numerator_histogram, ROOT.TGraph) and isinstance(denominator_histogram, ROOT.TF1)) or
			    (isinstance(numerator_histogram, ROOT.TF1) and isinstance(denominator_histogram, ROOT.TGraph))):
				graph = numerator_histogram if isinstance(numerator_histogram, ROOT.TGraph) else denominator_histogram
				function = denominator_histogram if isinstance(numerator_histogram, ROOT.TGraph) else numerator_histogram
				function_graph = graph.Clone("function_graph_"+function.GetName())
				scaleerrors.ScaleErrors.scale_errors(function_graph, scale_factor=0.0)
				
				x_values = function_graph.GetX()
				x_values = [x_values[index] for index in xrange(function_graph.GetN())]
				if not isinstance(function_graph, ROOT.TGraph2D):
					y_values = [function.Eval(x_value) for x_value in x_values]
					for index, (x_value, y_value) in enumerate(zip(x_values, y_values)):
						function_graph.SetPoint(index, x_value, y_value)
				else:
					y_values = function_graph.GetY()
					y_values = [y_values[index] for index in xrange(function_graph.GetN())]
					
					z_values = [function.Eval(x_value, y_value) for x_value, y_value in zip(x_values, y_values)]
					for index, (x_value, y_value, z_value) in enumerate(zip(x_values, y_values, z_values)):
						function_graph.SetPoint(index, x_value, y_value, z_value)
				
				if isinstance(numerator_histogram, ROOT.TGraph):
					denominator_histogram = function_graph
				else:
					numerator_histogram = function_graph
			
			# Calculate ratio
			if isinstance(numerator_histogram, ROOT.TGraph) and isinstance(denominator_histogram, ROOT.TGraph):

				if isinstance(numerator_histogram, ROOT.TGraphAsymmErrors) and isinstance(denominator_histogram, ROOT.TGraphAsymmErrors):
					ratio_histogram = ROOT.TGraphAsymmErrors()
				elif isinstance(numerator_histogram, ROOT.TGraphErrors) and isinstance(denominator_histogram, ROOT.TGraphErrors):
					ratio_histogram = ROOT.TGraphErrors()
				else:
					ratio_histogram = ROOT.TGraph()

				successful_division = True
				for point in range(0, numerator_histogram.GetN()):

					x_value = ROOT.Double(0)
					y_value_numerator = ROOT.Double(0)
					numerator_histogram.GetPoint(point, x_value, y_value_numerator)
					y_value_denominator = denominator_histogram.Eval(x_value)

					if y_value_denominator != 0.:

						ratio = y_value_numerator / y_value_denominator
						ratio_histogram.SetPoint(point, x_value, ratio)

						if isinstance(ratio_histogram, ROOT.TGraphAsymmErrors):

							x_err_high = numerator_histogram.GetErrorXhigh(point)
							x_err_low = numerator_histogram.GetErrorXlow(point)

							y_err_high_rel_numerator = numerator_histogram.GetErrorYhigh(point) / y_value_numerator
							y_err_low_rel_numerator = numerator_histogram.GetErrorYlow(point) / y_value_numerator

							y_err_high_rel_denominator = denominator_histogram.GetErrorYhigh(point) / y_value_denominator
							y_err_low_rel_denominator = denominator_histogram.GetErrorYlow(point) / y_value_denominator

							y_err_high_ratio = sqrt(pow(ratio, 2.0) * (pow(y_err_high_rel_numerator, 2) + pow(y_err_high_rel_denominator, 2)))
							y_err_low_ratio = sqrt(pow(ratio, 2.0) * (pow(y_err_low_rel_numerator, 2) + pow(y_err_low_rel_denominator, 2)))

							ratio_histogram.SetPointEXhigh(point, x_err_high)
							ratio_histogram.SetPointEXlow(point, x_err_low)
							ratio_histogram.SetPointEYhigh(point, y_err_high_ratio)
							ratio_histogram.SetPointEYlow(point, y_err_low_ratio)

						elif isinstance(ratio_histogram, ROOT.TGraphErrors):

							x_err = numerator_histogram.GetErrorX(point) / y_value_numerator
							y_err_rel_numerator = numerator_histogram.GetErrorY(point) / y_value_denominator
							y_err_rel_denominator = denominator_histogram.GetErrorY(point)
							y_err_ratio = sqrt(pow(ratio, 2.0) * (pow(y_err_rel_numerator, 2) + pow(y_err_rel_denominator, 2)))
							ratio_histogram.SetPointError(point, x_err, y_err_ratio)

						else: log.fatal("ratio_histogram is neither ROOT.TGraphAsymmErrors nor ROOT.TGraphErrors")

					else:
						log.fatal("Division by ZERO")
						successful_division = False
			else:
				ratio_histogram = roottools.RootTools.to_histogram(numerator_histogram)
				successful_division = ratio_histogram.Divide(roottools.RootTools.to_histogram(denominator_histogram))

			if not successful_division:
				if isinstance(ratio_histogram, ROOT.TGraph):
					log.warning("Could not successfully divide all points of the graphs '{0}' and '{1}'!".format(",".join(ratio_numerator_nicks), ",".join(ratio_denominator_nicks)))
				else:
					log.warning("Could not successfully divide histogram(s) '{0}' by '{1}'!".format(",".join(ratio_numerator_nicks), ",".join(ratio_denominator_nicks)))

			if hasattr(ratio_histogram, "SetDirectory"): ratio_histogram.SetDirectory(0)
			ratio_histogram.SetTitle("")

			plotData.plotdict["root_objects"][ratio_result_nick] = ratio_histogram
