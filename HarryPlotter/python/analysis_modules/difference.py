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


class Difference(analysisbase.AnalysisBase):
	"""Add differences of histograms"""

	def modify_argument_parser(self, parser, args):
		super(Difference, self).modify_argument_parser(parser, args)

		self.difference_options = parser.add_argument_group("{} options".format(self.name()))
		self.difference_options.add_argument("--difference-minuend-nicks", nargs="+",
				help="Nick names for the minuends of the difference. Multiple nicks (whitespace separated) will be summed up before calculating the difference.")
		self.difference_options.add_argument("--difference-subtrahend-nicks", nargs="+",
				help="Nick names for the subtrahend of the difference. Multiple nicks (whitespace separated) will be summed up before calculating the difference.")
		self.difference_options.add_argument("--difference-result-nicks", nargs="+",
				help="Nick names for the resulting difference graphs.")

	def prepare_args(self, parser, plotData):
		super(Difference, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["difference_minuend_nicks", "difference_subtrahend_nicks", "difference_result_nicks"])
		self.auto_set_arguments(plotData, ["difference_minuend_nicks", "difference_subtrahend_nicks"], "difference_result_nicks", "difference")

		for index, (difference_minuend_nick, difference_subtrahend_nick, difference_result_nick) in enumerate(zip(*[plotData.plotdict[k] for k in ["difference_minuend_nicks", "difference_subtrahend_nicks", "difference_result_nicks"]])):
			plotData.plotdict["difference_minuend_nicks"][index] = difference_minuend_nick.split()
			plotData.plotdict["difference_subtrahend_nicks"][index] = difference_subtrahend_nick.split()

			if difference_result_nick is None:
				plotData.plotdict["difference_result_nicks"][index] = "difference_{num}_over_{denom}".format(
						num="_".join(plotData.plotdict["difference_minuend_nicks"][index]),
						denom="_".join(plotData.plotdict["difference_subtrahend_nicks"][index]))

			if not plotData.plotdict["difference_result_nicks"][index] in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].append(plotData.plotdict["difference_result_nicks"][index])

	@staticmethod
	def get_histograms(plotdict={}, difference_nicks=None, difference_minuend_nicks=None, difference_subtrahend_nicks=None, difference_result_nick=None):
		histogram = None

		for nick in difference_nicks:

			root_object = plotdict[nick]
			if histogram is None:
				histogram = root_object.Clone(
						"difference_" + hashlib.md5("_".join([str(difference_nicks), str(difference_subtrahend_nicks), difference_result_nick])).hexdigest()
				)
			else: histogram.Add(root_object)

			if hasattr(histogram, "SetDirectory"): histogram.SetDirectory(0)

		return histogram

	def run(self, plotData=None):
		super(Difference, self).run(plotData)

		for difference_minuend_nicks, difference_subtrahend_nicks, difference_result_nick in zip(
				*[plotData.plotdict[k] for k in ["difference_minuend_nicks", "difference_subtrahend_nicks", "difference_result_nicks"]]):

			new_name = "histogram_" + hashlib.md5("_".join(map(str, [difference_minuend_nicks, difference_subtrahend_nicks, difference_result_nick]))).hexdigest()

			# Create nick sum histograms
			minuend_histogram = self.get_histograms(plotdict=plotData.plotdict["root_objects"], difference_nicks=difference_minuend_nicks, difference_minuend_nicks=difference_minuend_nicks, difference_subtrahend_nicks=difference_subtrahend_nicks, difference_result_nick=difference_result_nick)
			subtrahend_histogram = self.get_histograms(plotdict=plotData.plotdict["root_objects"], difference_nicks=difference_subtrahend_nicks, difference_minuend_nicks=difference_minuend_nicks, difference_subtrahend_nicks=difference_subtrahend_nicks, difference_result_nick=difference_result_nick)

			# preparations for differences with one function
			if ((isinstance(minuend_histogram, ROOT.TGraph) and isinstance(subtrahend_histogram, ROOT.TF1)) or
			    (isinstance(minuend_histogram, ROOT.TF1) and isinstance(subtrahend_histogram, ROOT.TGraph))):
				graph = minuend_histogram if isinstance(minuend_histogram, ROOT.TGraph) else subtrahend_histogram
				function = subtrahend_histogram if isinstance(minuend_histogram, ROOT.TGraph) else minuend_histogram
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
				
				if isinstance(minuend_histogram, ROOT.TGraph):
					subtrahend_histogram = function_graph
				else:
					minuend_histogram = function_graph
			
			# Calculate difference
			if isinstance(minuend_histogram, ROOT.TGraph) and isinstance(subtrahend_histogram, ROOT.TGraph):

				if isinstance(minuend_histogram, ROOT.TGraphAsymmErrors) and isinstance(subtrahend_histogram, ROOT.TGraphAsymmErrors):
					difference_histogram = ROOT.TGraphAsymmErrors()
				elif isinstance(minuend_histogram, ROOT.TGraphErrors) and isinstance(subtrahend_histogram, ROOT.TGraphErrors):
					difference_histogram = ROOT.TGraphErrors()
				else:
					difference_histogram = ROOT.TGraph()

				for point in range(0, minuend_histogram.GetN()):

					x_value = ROOT.Double(0)
					y_value_minuend = ROOT.Double(0)
					minuend_histogram.GetPoint(point, x_value, y_value_minuend)
					y_value_subtrahend = subtrahend_histogram.Eval(x_value)

					difference = y_value_minuend - y_value_subtrahend
					difference_histogram.SetPoint(point, x_value, difference)

					if isinstance(difference_histogram, ROOT.TGraphAsymmErrors):

						x_err_high = minuend_histogram.GetErrorXhigh(point)
						x_err_low = minuend_histogram.GetErrorXlow(point)

						y_err_high_minuend = minuend_histogram.GetErrorYhigh(point)
						y_err_low_minuend = minuend_histogram.GetErrorYlow(point)

						y_err_high_subtrahend = subtrahend_histogram.GetErrorYhigh(point)
						y_err_low_subtrahend = subtrahend_histogram.GetErrorYlow(point)

						y_err_high_difference = sqrt(pow(y_err_high_minuend, 2) + pow(y_err_high_subtrahend, 2))
						y_err_low_difference = sqrt(pow(y_err_low_minuend, 2) + pow(y_err_low_subtrahend, 2))

						difference_histogram.SetPointEXhigh(point, x_err_high)
						difference_histogram.SetPointEXlow(point, x_err_low)
						difference_histogram.SetPointEYhigh(point, y_err_high_difference)
						difference_histogram.SetPointEYlow(point, y_err_low_difference)

					elif isinstance(difference_histogram, ROOT.TGraphErrors):

						x_err = minuend_histogram.GetErrorX(point)
						y_err_minuend = minuend_histogram.GetErrorY(point)
						y_err_subtrahend = subtrahend_histogram.GetErrorY(point)
						y_err_difference = sqrt(pow(y_err_minuend, 2) + pow(y_err_subtrahend, 2))
						difference_histogram.SetPointError(point, x_err, y_err_difference)

					else: log.fatal("difference_histogram is neither ROOT.TGraphAsymmErrors nor ROOT.TGraphErrors")

			else:
				minuend_histogram = roottools.RootTools.to_histogram(minuend_histogram)
				subtrahend_histogram = roottools.RootTools.to_histogram(subtrahend_histogram)
				
				# check for same binning
				minuend_histogram_binning = [roottools.RootTools.get_binning(minuend_histogram, 0), roottools.RootTools.get_binning(minuend_histogram, 1), roottools.RootTools.get_binning(minuend_histogram, 2)]
				subtrahend_histogram_binning = [roottools.RootTools.get_binning(subtrahend_histogram, 0), roottools.RootTools.get_binning(subtrahend_histogram, 1), roottools.RootTools.get_binning(subtrahend_histogram, 2)]
				
				minuend_histogram_n_bins = reduce(lambda a, b: a*b, map(len, minuend_histogram_binning))
				subtrahend_histogram_n_bins = reduce(lambda a, b: a*b, map(len, subtrahend_histogram_binning))
				
				for axis in range(minuend_histogram.GetDimension(), 3):
					minuend_histogram_binning[axis] = 1
				for axis in range(subtrahend_histogram.GetDimension(), 3):
					subtrahend_histogram_binning[axis] = 1
				
				if minuend_histogram_n_bins == subtrahend_histogram_n_bins:
					difference_histogram = minuend_histogram.Clone(new_name)
					difference_histogram.Add(subtrahend_histogram, -1.0)
				else:
					if minuend_histogram_n_bins < subtrahend_histogram_n_bins:
						difference_histogram = subtrahend_histogram.Clone(new_name)
					else:
						difference_histogram = minuend_histogram.Clone(new_name)
					
					difference_histogram.Reset()
					for x_bin in xrange(0, difference_histogram.GetNbinsX()+2):
						x_bin_center = difference_histogram.GetXaxis().GetBinCenter(x_bin)
						x_bin_minuend = minuend_histogram.GetXaxis().FindBin(x_bin_center)
						x_bin_subtrahend = subtrahend_histogram.GetXaxis().FindBin(x_bin_center)
						
						if difference_histogram.GetDimension() > 1:
							for y_bin in xrange(0, difference_histogram.GetNbinsY()+2):
								y_bin_center = difference_histogram.GetYaxis().GetBinCenter(y_bin)
								y_bin_minuend = minuend_histogram.GetYaxis().FindBin(y_bin_center)
								y_bin_subtrahend = subtrahend_histogram.GetYaxis().FindBin(y_bin_center)
								
								if difference_histogram.GetDimension() > 2:
									for z_bin in xrange(0, difference_histogram.GetNbinsZ()+2):
										z_bin_center = difference_histogram.GetZaxis().GetBinCenter(z_bin)
										z_bin_minuend = minuend_histogram.GetZaxis().FindBin(z_bin_center)
										z_bin_subtrahend = subtrahend_histogram.GetZaxis().FindBin(z_bin_center)
										
										ratio, ratio_error = Difference.subtract_bin_contents(minuend_histogram, [x_bin_minuend, y_bin_minuend, z_bin_minuend], subtrahend_histogram, [x_bin_subtrahend, y_bin_subtrahend, z_bin_subtrahend])
										difference_histogram.SetBinContent(x_bin, y_bin, z_bin, ratio)
										difference_histogram.SetBinError(x_bin, y_bin, z_bin, ratio_error)
								
								else:
									ratio, ratio_error = Difference.subtract_bin_contents(minuend_histogram, [x_bin_minuend, y_bin_minuend], subtrahend_histogram, [x_bin_subtrahend, y_bin_subtrahend])
									difference_histogram.SetBinContent(x_bin, y_bin, ratio)
									difference_histogram.SetBinError(x_bin, y_bin, ratio_error)
						
						else:
							ratio, ratio_error = Difference.subtract_bin_contents(minuend_histogram, [x_bin_minuend], subtrahend_histogram, [x_bin_subtrahend])
							difference_histogram.SetBinContent(x_bin, ratio)
							difference_histogram.SetBinError(x_bin, ratio_error)
			
			if hasattr(difference_histogram, "SetDirectory"): difference_histogram.SetDirectory(0)
			difference_histogram.SetTitle("")

			plotData.plotdict["root_objects"][difference_result_nick] = difference_histogram

	@staticmethod
	def subtract_bin_contents(minuend_histogram, minuend_bins, subtrahend_histogram, subtrahend_bins):
		content_minuend = minuend_histogram.GetBinContent(*minuend_bins)
		error_minuend = minuend_histogram.GetBinError(*minuend_bins)
		content_subtrahend = subtrahend_histogram.GetBinContent(*subtrahend_bins)
		error_subtrahend = subtrahend_histogram.GetBinError(*subtrahend_bins)
		
		difference = content_minuend - content_subtrahend
		difference_error = sqrt(pow(error_minuend, 2) + pow(error_subtrahend, 2))
		
		return difference, difference_error

