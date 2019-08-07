# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy
import hashlib
import math

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.analysis_modules.scaleerrors as scaleerrors


class Divide(analysisbase.AnalysisBase):
	"""Divide selected histogram(s) (numerator) by other histogram(s) (denominator).
		Histograms are not divided in-place, but copies are created (result-nicks)."""

	def modify_argument_parser(self, parser, args):
		super(Divide, self).modify_argument_parser(parser, args)

		self.divide_options = parser.add_argument_group("{} options".format(self.name()))
		self.divide_options.add_argument("--divide-numerator-nicks", type=str, nargs="+",
				help="Nick names for the numerators of the divide.")
		self.divide_options.add_argument("--divide-denominator-nicks", type=str, nargs="+",
				help="Nick names for the denominator of the divide.")
		self.divide_options.add_argument("--divide-result-nicks", type=str, nargs="+",
				help="Nick names for the resulting divide graphs.")
		self.divide_options.add_argument("--divide-tgraphs", nargs="+", type="bool", default=[False],
				help="Do TGraph division instead of implicite conversion to Histogram.")
		self.divide_options.add_argument("--divide-denominator-no-errors", nargs="+", type="bool", default=[False],
				help="Remove errors from denominator histo before division.")
		
	def prepare_args(self, parser, plotData):
		super(Divide, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["divide_numerator_nicks", "divide_denominator_nicks", "divide_result_nicks", "divide_tgraphs", "divide_denominator_no_errors"])
		self.auto_set_arguments(plotData, ["divide_numerator_nicks", "divide_denominator_nicks"],
				"divide_result_nicks", "divide")
		
	def divide_tgraph(self, graph1, graph2):
			assert (graph1.GetN() == graph2.GetN())
			for i in xrange(graph1.GetN()):
					graph1X, graph1Y = ROOT.Double(0), ROOT.Double(0)
					graph1.GetPoint(i, graph1X, graph1Y)
					graph2X, graph2Y = ROOT.Double(0), ROOT.Double(0)
					graph2.GetPoint(i, graph2X, graph2Y)
					graph1.SetPoint(i, graph1X, graph1Y / graph2Y if graph2Y != 0. else 0.)
					if isinstance(graph1, ROOT.TGraphAsymmErrors):
						graph1.SetPointEYlow(i, abs(graph1.GetErrorYlow(i) / graph2Y) if graph2Y != 0. else 0.)
					graph1.SetPointEYhigh(i, abs(graph1.GetErrorYhigh(i) / graph2Y) if graph2Y != 0. else 0.)
				else:
				graph1.SetPointError(i, 0.0, abs(graph1.GetErrorYlow(i) / graph2Y) if graph2Y != 0. else 0.)

	def run(self, plotData=None):
		super(Divide, self).run(plotData)

		for numerator, denominator, new, tgraph, denominator_no_errors in zip(
			plotData.plotdict["divide_numerator_nicks"],
			plotData.plotdict["divide_denominator_nicks"],
			plotData.plotdict["divide_result_nicks"],
			plotData.plotdict["divide_tgraphs"],
			plotData.plotdict["divide_denominator_no_errors"]
		):
			log.debug("Dividing %s by %s (new nick: %s)" % (numerator, denominator, new))
			new_name = "histogram_" + hashlib.md5("_".join([numerator, denominator, new])).hexdigest()
			numerator_histogram = plotData.plotdict["root_objects"][numerator]
			denominator_histogram = plotData.plotdict["root_objects"][denominator]
			if tgraph:
				if isinstance(numerator_histogram,ROOT.TGraph) and isinstance(denominator_histogram,ROOT.TGraph):
					new_histogram = plotData.plotdict["root_objects"][numerator].Clone(new_name)
					self.divide_tgraph(new_histogram, denominator_histogram)
				else:
					log.error("Divide: Chose TGraph division but input is no TGraph!")
			else:
				numerator_histogram = roottools.RootTools.to_histogram(numerator_histogram)
				denominator_histogram = roottools.RootTools.to_histogram(denominator_histogram)
				
				if denominator_no_errors:
					scaleerrors.ScaleErrors.scale_errors(denominator_histogram)
				
				# check for same binning
				numerator_histogram_binning = [roottools.RootTools.get_binning(numerator_histogram, 0), roottools.RootTools.get_binning(numerator_histogram, 1), roottools.RootTools.get_binning(numerator_histogram, 2)]
				denominator_histogram_binning = [roottools.RootTools.get_binning(denominator_histogram, 0), roottools.RootTools.get_binning(denominator_histogram, 1), roottools.RootTools.get_binning(denominator_histogram, 2)]
				
				numerator_histogram_n_bins = reduce(lambda a, b: a*b, map(len, numerator_histogram_binning))
				denominator_histogram_n_bins = reduce(lambda a, b: a*b, map(len, denominator_histogram_binning))
				
				for axis in range(numerator_histogram.GetDimension(), 3):
					numerator_histogram_binning[axis] = 1
				for axis in range(denominator_histogram.GetDimension(), 3):
					denominator_histogram_binning[axis] = 1
				
				if numerator_histogram_n_bins == denominator_histogram_n_bins:
					new_histogram = numerator_histogram.Clone(new_name)
					new_histogram.Divide(denominator_histogram)
				else:
					if numerator_histogram_n_bins < denominator_histogram_n_bins:
						new_histogram = denominator_histogram.Clone(new_name)
					else:
						new_histogram = numerator_histogram.Clone(new_name)
					
					new_histogram.Reset()
					for x_bin in xrange(0, new_histogram.GetNbinsX()+2):
						x_bin_center = new_histogram.GetXaxis().GetBinCenter(x_bin)
						x_bin_numerator = numerator_histogram.GetXaxis().FindBin(x_bin_center)
						x_bin_denominator = denominator_histogram.GetXaxis().FindBin(x_bin_center)
						
						if new_histogram.GetDimension() > 1:
							for y_bin in xrange(0, new_histogram.GetNbinsY()+2):
								y_bin_center = new_histogram.GetYaxis().GetBinCenter(y_bin)
								y_bin_numerator = numerator_histogram.GetYaxis().FindBin(y_bin_center)
								y_bin_denominator = denominator_histogram.GetYaxis().FindBin(y_bin_center)
								
								if new_histogram.GetDimension() > 2:
									for z_bin in xrange(0, new_histogram.GetNbinsZ()+2):
										z_bin_center = new_histogram.GetZaxis().GetBinCenter(z_bin)
										z_bin_numerator = numerator_histogram.GetZaxis().FindBin(z_bin_center)
										z_bin_denominator = denominator_histogram.GetZaxis().FindBin(z_bin_center)
										
										ratio, ratio_error = Divide.divide_bin_contents(numerator_histogram, [x_bin_numerator, y_bin_numerator, z_bin_numerator], denominator_histogram, [x_bin_denominator, y_bin_denominator, z_bin_denominator])
										new_histogram.SetBinContent(x_bin, y_bin, z_bin, ratio)
										new_histogram.SetBinError(x_bin, y_bin, z_bin, ratio_error)
								
								else:
									ratio, ratio_error = Divide.divide_bin_contents(numerator_histogram, [x_bin_numerator, y_bin_numerator], denominator_histogram, [x_bin_denominator, y_bin_denominator])
									new_histogram.SetBinContent(x_bin, y_bin, ratio)
									new_histogram.SetBinError(x_bin, y_bin, ratio_error)
						
						else:
							ratio, ratio_error = Divide.divide_bin_contents(numerator_histogram, [x_bin_numerator], denominator_histogram, [x_bin_denominator])
							new_histogram.SetBinContent(x_bin, ratio)
							new_histogram.SetBinError(x_bin, ratio_error)
							
			plotData.plotdict["root_objects"][new] = new_histogram
			plotData.plotdict["nicks"].append(new)

	@staticmethod
	def divide_bin_contents(numerator_histogram, numerator_bins, denominator_histogram, denominator_bins):
		content_numerator = numerator_histogram.GetBinContent(*numerator_bins)
		error_numerator = numerator_histogram.GetBinError(*numerator_bins)
		content_denominator = denominator_histogram.GetBinContent(*denominator_bins)
		error_denominator = denominator_histogram.GetBinError(*denominator_bins)
		
		ratio = 0.0 if content_denominator == 0.0 else (content_numerator / content_denominator)
		ratio_error = 0.0 if content_numerator*content_denominator == 0.0 else (abs(ratio) * math.sqrt(pow(error_numerator/content_numerator, 2) + pow(error_denominator/content_denominator, 2)))
		
		return ratio, ratio_error

