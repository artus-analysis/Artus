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

	def run(self, plotData=None):
		super(NormalizeByBinWidth, self).run(plotData)
		
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
				sumBinContentColumn = 0
				for iBinX in range(1, root_histogram.GetNbinsX()+1):
					for iBinY in range(1, root_histogram.GetNbinsY()+1):
						sumBinContentColumn += root_histogram.GetBinContent(iBinX, iBinY)
					for iBinY in range(1, root_histogram.GetNbinsY()+1):
						binContent = root_histogram.GetBinContent(iBinX, iBinY)
						root_histogram.SetBinContent(iBinX, iBinY, binContent/sumBinContentColumn)
					sumBinContentColumn = 0

class NormalizeRowsToUnity(analysisbase.AnalysisBase):
	"""In a TH2 histo, normalize each row such that sum(columns) = 1."""
	def __init__(self):
		super(NormalizeRowsToUnity, self).__init__()

	def run(self, plotData=None):
		super(NormalizeRowsToUnity, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH2):
				root_histogram.Sumw2()
				sumBinContentRow = 0
				for iBinY in range(1, root_histogram.GetNbinsY()+1):
					for iBinX in range(1, root_histogram.GetNbinsX()+1):
						sumBinContentRow += root_histogram.GetBinContent(iBinX, iBinY)
					for iBinX in range(1, root_histogram.GetNbinsX()+1):
						binContent = root_histogram.GetBinContent(iBinX, iBinY)
						root_histogram.SetBinContent(iBinX, iBinY, binContent/sumBinContentRow)
					sumBinContentRow = 0

class NormalizeToFirstHisto(analysisbase.AnalysisBase):
	"""Normalize histograms to first histogram."""
	def __init__(self):
		super(NormalizeToFirstHisto, self).__init__()

	def run(self, plotData=None):
		super(NormalizeToFirstHisto, self).run(plotData)
		refhisto_int = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]].Integral()

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1):
				root_histogram.Sumw2()
				root_histogram.Scale(refhisto_int / root_histogram.Integral())

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
				root_histogram.Scale(refhisto_int / stack_int)
