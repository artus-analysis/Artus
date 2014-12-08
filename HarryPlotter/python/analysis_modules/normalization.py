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
