# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class NormalizeByBinWidth(analysisbase.AnalysisBase):
	"""Normalize all histograms by bin width."""
	def __init__(self):
		super(NormalizeByBinWidth, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(NormalizeByBinWidth, self).modify_argument_parser(parser, args)

		self.normalizebybinwidth_options = parser.add_argument_group("{} options".format(self.name()))
		self.normalizebybinwidth_options.add_argument("--histograms-to-normalize-by-binwidth", type=str, nargs="+",
				help="Nick names of the histogram to normalize to be normalized")

	def prepare_args(self, parser, plotData):
		super(NormalizeByBinWidth, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["histograms_to_normalize_by_binwidth"])

	def run(self, plotData=None):
		super(NormalizeByBinWidth, self).run(plotData)
		print plotData.plotdict["histograms_to_normalize_by_binwidth"]
		if plotData.plotdict["histograms_to_normalize_by_binwidth"] != [None]:
			for histo_to_normalize in plotData.plotdict["histograms_to_normalize_by_binwidth"]:
				root_histogram = plotData.plotdict["root_objects"][histo_to_normalize]
				if isinstance(root_histogram, ROOT.TH1):
					root_histogram.Sumw2()
					root_histogram.Scale(1.0, "width")
		
		else:
			for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
				if isinstance(root_histogram, ROOT.TH1):
					root_histogram.Sumw2()
					root_histogram.Scale(1.0, "width")

class NormalizeToUnity(analysisbase.AnalysisBase):
	"""Normalize all histograms to unity."""
	def __init__(self):
		super(NormalizeToUnity, self).__init__()

	def run(self, plotData=None):
		super(NormalizeToUnity, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1):
				root_histogram.Sumw2()
				if root_histogram.Integral() != 0.0:
					root_histogram.Scale(1.0 / root_histogram.Integral())

class NormalizeColumnsToUnity(analysisbase.AnalysisBase):
	"""In a TH2 histo, normalize each column such that sum(rows) = 1."""
	def __init__(self):
		super(NormalizeColumnsToUnity, self).__init__()

	def run(self, plotData=None):
		super(NormalizeColumnsToUnity, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH2):
				root_histogram.Sumw2()
				sumBinContentColumn = 0.0
				for iBinX in range(1, root_histogram.GetNbinsX()+1):
					for iBinY in range(1, root_histogram.GetNbinsY()+1):
						sumBinContentColumn += root_histogram.GetBinContent(iBinX, iBinY)
					for iBinY in range(1, root_histogram.GetNbinsY()+1):
						binContent = root_histogram.GetBinContent(iBinX, iBinY)
						binError = root_histogram.GetBinError(iBinX, iBinY)
						root_histogram.SetBinContent(iBinX, iBinY, binContent/sumBinContentColumn)
						root_histogram.SetBinError(iBinX, iBinY, binError/sumBinContentColumn)
					sumBinContentColumn = 0.0

class NormalizeRowsToUnity(analysisbase.AnalysisBase):
	"""In a TH2 histo, normalize each row such that sum(columns) = 1."""
	def __init__(self):
		super(NormalizeRowsToUnity, self).__init__()

	def run(self, plotData=None):
		super(NormalizeRowsToUnity, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH2):
				root_histogram.Sumw2()
				sumBinContentRow = 0.0
				for iBinY in range(1, root_histogram.GetNbinsY()+1):
					for iBinX in range(1, root_histogram.GetNbinsX()+1):
						sumBinContentRow += root_histogram.GetBinContent(iBinX, iBinY)
					for iBinX in range(1, root_histogram.GetNbinsX()+1):
						binContent = root_histogram.GetBinContent(iBinX, iBinY)
						binError = root_histogram.GetBinError(iBinX, iBinY)
						root_histogram.SetBinContent(iBinX, iBinY, binContent/sumBinContentRow)
						root_histogram.SetBinError(iBinX, iBinY, binError/sumBinContentColumn)
					sumBinContentRow = 0.0


class NormalizeHistogram(analysisbase.AnalysisBase):
	"""Normalize specified histograms to another specified histogram."""
	def __init__(self):
		super(NormalizeHistogram, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(NormalizeHistogram, self).modify_argument_parser(parser, args)

		self.normalize_histogram_options = parser.add_argument_group("{} options".format(self.name()))
		self.normalize_histogram_options.add_argument("--normalization-base-histo", type=str, nargs="+",
				help="Nick names of the histogram to normalize to.")
		self.normalize_histogram_options.add_argument("--histograms-to-normalize", type=str, nargs="+",
				help="Nick names of the histogram to normalize to be normalized")

	def prepare_args(self, parser, plotData):
		super(NormalizeHistogram, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["normalization_base_histo", "histograms_to_normalize"])


	def normalize_histogram(self, refhisto_nicks, nicks_to_normalize, plotData=None ):
		for refhisto, histos_to_normalize in zip(refhisto_nicks, nicks_to_normalize):
			refhisto_int = plotData.plotdict["root_objects"][refhisto].Integral()

			for histo_to_normalize in histos_to_normalize.split(" "):
				root_histogram = plotData.plotdict["root_objects"][histo_to_normalize]
				if isinstance(root_histogram, ROOT.TH1):
					root_histogram.Sumw2()
					if root_histogram.Integral() != 0.0:
						log.debug("{0}: Scaling histogram {1} by {2}".format(self.name(), histo_to_normalize, (refhisto_int / root_histogram.Integral())))
						root_histogram.Scale(refhisto_int / root_histogram.Integral())

	def run(self, plotData=None):
		super(NormalizeHistogram, self).run(plotData)
		self.normalize_histogram(
		                    refhisto_nicks = plotData.plotdict["normalization_base_histo"],
		                    nicks_to_normalize = plotData.plotdict["histograms_to_normalize"],
		                    plotData = plotData)

class NormalizeToFirstHisto(NormalizeHistogram):
	"""Normalize histograms to first histogram."""
	def run(self, plotData=None):
		self.normalize_histogram(
		                    refhisto_nicks = [plotData.plotdict["nicks"][0]], # take nickname at first position
		                    nicks_to_normalize = [" ".join(plotData.plotdict["nicks"][1:])], # all other nicknames apart from the first one. Join them to one string to be consistent with parser
		                    plotData=plotData)

class NormalizeStackToFirstHisto(analysisbase.AnalysisBase):
	"""Normalize stacked histograms to first histogram."""
	def __init__(self):
		super(NormalizeStackToFirstHisto, self).__init__()

	def run(self, plotData=None):
		super(NormalizeStackToFirstHisto, self).run(plotData)
		refhisto_int = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]].Integral()

		stack_int = 0
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1) and nick != plotData.plotdict["nicks"][0]:
				stack_int += root_histogram.Integral()

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1) and nick != plotData.plotdict["nicks"][0]:
				root_histogram.Sumw2()
				if stack_int != 0.0:
					root_histogram.Scale(refhisto_int / stack_int)

class NormalizeStacksToUnity(analysisbase.AnalysisBase):
	"""Normalize Stacks to Unity."""
	def __init__(self):
		super(NormalizeStacksToUnity, self).__init__()

	def run(self, plotData=None):
		super(NormalizeStacksToUnity, self).run(plotData)
		plot_nicks = []
		plot_stacks = plotData.plotdict["stacks"]
		stack_values = {}
		for nick in plotData.plotdict["nicks"]:
			if not max([black in nick for black in plotData.plotdict.get("nicks_blacklist", [])]+[False]):
				plot_nicks.append(nick)
		for stack, nick in zip(plot_stacks, plot_nicks):
			if not stack in stack_values.keys():
				stack_values[stack] = plotData.plotdict["root_objects"][nick].Integral(0, plotData.plotdict["root_objects"][nick].GetNbinsX())
			else:
				stack_values[stack] += plotData.plotdict["root_objects"][nick].Integral()
		for stack, nick in zip(plot_stacks, plot_nicks):
			plotData.plotdict["root_objects"][nick].Scale(1./stack_values[stack])
