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
		self.ratio_options.add_argument(
				"--ratio-numerator-nicks", nargs="+",
				help="Nick names for the numerators of the ratio. Multiple nicks (whitespace separated) will be summed up before calculating the ratio."
		)
		self.ratio_options.add_argument(
				"--ratio-numerator-no-errors", nargs="+", type="bool", default=[False],
				help="Remove errors of numerator histograms before calculating the ratio. [Default: %(default)s]"
		)
		self.ratio_options.add_argument(
				"--ratio-denominator-nicks", nargs="+",
				help="Nick names for the denominator of the ratio. Multiple nicks (whitespace separated) will be summed up before calculating the ratio."
		)
		self.ratio_options.add_argument(
				"--ratio-denominator-no-errors", nargs="+", type="bool", default=[True],
				help="Remove errors of denominator histograms before calculating the ratio. [Default: %(default)s]"
		)
		self.ratio_options.add_argument(
				"--ratio-result-nicks", nargs="+",
				help="Nick names for the resulting ratio graphs."
		)

	def prepare_args(self, parser, plotData):
		super(Ratio, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["ratio_numerator_nicks", "ratio_numerator_no_errors", "ratio_denominator_nicks", "ratio_denominator_no_errors", "ratio_result_nicks"])
		self.auto_set_arguments(plotData, ["ratio_numerator_nicks", "ratio_denominator_nicks"], "ratio_result_nicks", "ratio")
		
		for index, (ratio_numerator_nick, ratio_denominator_nick, ratio_result_nick) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["ratio_numerator_nicks", "ratio_denominator_nicks", "ratio_result_nicks"]]
		)):
			plotData.plotdict["ratio_numerator_nicks"][index] = ratio_numerator_nick.split()
			plotData.plotdict["ratio_denominator_nicks"][index] = ratio_denominator_nick.split()
			if ratio_result_nick is None:
				plotData.plotdict["ratio_result_nicks"][index] = "ratio_{num}_over_{denom}".format(
						num="_".join(plotData.plotdict["ratio_numerator_nicks"][index]),
						denom="_".join(plotData.plotdict["ratio_denominator_nicks"][index])
				)
			if not plotData.plotdict["ratio_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].append(plotData.plotdict["ratio_result_nicks"][index])

	def run(self, plotData=None):
		super(Ratio, self).run(plotData)
		
		for ratio_numerator_nicks, ratio_numerator_no_errors, ratio_denominator_nicks, ratio_denominator_no_errors, ratio_result_nick in zip(
				*[plotData.plotdict[k] for k in ["ratio_numerator_nicks", "ratio_numerator_no_errors", "ratio_denominator_nicks", "ratio_denominator_no_errors", "ratio_result_nicks"]]
		):
			# create nick sum histograms
			numerator_histogram = None
			for nick in ratio_numerator_nicks:
				root_object = plotData.plotdict["root_objects"][nick]
				if numerator_histogram is None:
					numerator_histogram = root_object.Clone(
							"ratio_" + hashlib.md5("_".join([str(ratio_numerator_nicks), str(ratio_denominator_nicks), ratio_result_nick])).hexdigest()
					)
				else:
					numerator_histogram.Add(root_object)
				if hasattr(numerator_histogram, "SetDirectory"):
					numerator_histogram.SetDirectory(0)
			
			if ratio_numerator_no_errors:
				scaleerrors.ScaleErrors.scale_errors(numerator_histogram)
			
			denominator_histogram = None
			for nick in ratio_denominator_nicks:
				root_object = plotData.plotdict["root_objects"][nick]
				if denominator_histogram is None:
					denominator_histogram = root_object.Clone(
							"ratio_denominator_" + hashlib.md5("_".join([str(ratio_numerator_nicks), str(ratio_denominator_nicks), ratio_result_nick])).hexdigest()
					)
				else:
					denominator_histogram.Add(root_object)
				if hasattr(denominator_histogram, "SetDirectory"):
					denominator_histogram.SetDirectory(0)
			
			if ratio_denominator_no_errors:
				scaleerrors.ScaleErrors.scale_errors(denominator_histogram)
			
			# calculate ratio
			if isinstance(numerator_histogram, ROOT.TGraph) and isinstance(denominator_histogram, ROOT.TGraph):
				if isinstance(numerator_histogram, ROOT.TGraphAsymmErrors) and isinstance(denominator_histogram, ROOT.TGraphAsymmErrors):
					ratio_histogram = ROOT.TGraphAsymmErrors()
				elif isinstance(numerator_histogram, ROOT.TGraphErrors) and isinstance(denominator_histogram, ROOT.TGraphErrors):
					ratio_histogram = ROOT.TGraphErrors()
				else:
					ratio_histogram = ROOT.TGraph()
				for point in range(0,numerator_histogram.GetN()):
					x_value = ROOT.Double(0)
					y_value_numerator = ROOT.Double(0)
					numerator_histogram.GetPoint(point, x_value, y_value_numerator)
					y_value_denominator = denominator_histogram.Eval(x_value)
					if y_value_denominator != 0.:
						ratio_histogram.SetPoint(point, x_value, y_value_numerator/y_value_denominator)
						if isinstance(ratio_histogram, ROOT.TGraphAsymmErrors):
							x_err_high = numerator_histogram.GetErrorXhigh(point)
							x_err_low = numerator_histogram.GetErrorXlow(point)
							y_err_high_numerator = numerator_histogram.GetErrorYhigh(point)
							y_err_high_denominator = denominator_histogram.GetErrorYhigh(point)
							y_err_low_numerator = numerator_histogram.GetErrorYlow(point)
							y_err_low_denominator = denominator_histogram.GetErrorYlow(point)
							y_err_high_ratio = sqrt(pow(y_err_high_numerator/y_value_denominator,2) + pow(x_value*y_err_high_denominator,2)/pow(y_value_denominator,4))
							y_err_low_ratio = sqrt(pow(y_err_low_numerator/y_value_denominator,2) + pow(x_value*y_err_low_denominator,2)/pow(y_value_denominator,4))
							ratio_histogram.SetPointEXhigh(point, x_err_high)
							ratio_histogram.SetPointEXlow(point, x_err_low)
							ratio_histogram.SetPointEYhigh(point, y_err_high_ratio)
							ratio_histogram.SetPointEYlow(point, y_err_low_ratio)
						elif isinstance(ratio_histogram, ROOT.TGraphErrors()):
							x_err = numerator_histogram.GetErrorX(point)
							y_err_numerator = numerator_histogram.GetErrorY(point)
							y_err_denominator = denominator_histogram.GetErrorY(point)
							y_err_ratio = sqrt(pow(y_err_numerator/y_value_denominator,2) + pow(x_value*y_err_denominator,2)/pow(y_value_denominator,4))
							ratio_histogram.SetPointError(point, x_err, y_err_ratio)
						successful_division = True
					else:
						successful_division = False
			else:
				ratio_histogram = roottools.RootTools.to_histogram(numerator_histogram)
				successful_division = ratio_histogram.Divide(roottools.RootTools.to_histogram(denominator_histogram))
			if not successful_division:
				if isinstance(ratio_histogram, ROOT.TGraph):
					log.warning("Could not successfully divide all points of the graphs '{0}' and '{1}'!".format(",".join(ratio_numerator_nicks), ",".join(ratio_denominator_nicks)))
				else:
					log.warning("Could not successfully divide histogram(s) '{0}' by '{1}'!".format(",".join(ratio_numerator_nicks), ",".join(ratio_denominator_nicks)))
			if hasattr(ratio_histogram, "SetDirectory"):
				ratio_histogram.SetDirectory(0)
			ratio_histogram.SetTitle("")
			plotData.plotdict["root_objects"][ratio_result_nick] = ratio_histogram

