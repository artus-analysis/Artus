# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import ROOT
import copy
import pprint
import Artus.HarryPlotter.analysisbase as analysisbase


class ProjectionY(analysisbase.AnalysisBase):
	""" Extract slices of 2D-Histograms, write them back to the dictionary and delete the original 2D Histogram. """

	def run(self, plotData=None):
		super(ProjectionY, self).run(plotData)
		old_nicks  = copy.deepcopy(plotData.plotdict["nicks"])
		old_labels = copy.deepcopy(plotData.plotdict["labels"])
		for nick, label in zip(old_nicks, old_labels):
			histogram_2D = plotData.plotdict["root_objects"][nick]
			if isinstance(histogram_2D, ROOT.TH2):
				slice_generator = self.histoSlice(histogram_2D, nick=nick, label=label)
				for slice in slice_generator:
					plotData.plotdict["root_objects"][slice.GetName()] = slice
					plotData.plotdict["nicks"].append(slice.GetName())
					plotData.plotdict["labels"].append(slice.GetTitle())
			plotData.plotdict["nicks"].remove(nick)
			plotData.plotdict["labels"].remove(label)
			del plotData.plotdict["root_objects"][nick]


	""" Generator to return slices of 2D histograms with proper labeling """
	@staticmethod
	def histoSlice(histogram_2D, nick = None, include_overflow = False, label = None):
		if include_overflow:
			iter_range = range(histogram_2D.GetNbinsX())
		else:
			iter_range = range(1, histogram_2D.GetNbinsX()+1, 1)
		if nick == None:
			nick = histogram_2D.GetName()
		for ibin in iter_range:
			slice = histogram_2D.ProjectionY(nick+"_slice_" + str(ibin), ibin, ibin+1)
			if label != None:
				slice.SetTitle( str(histogram_2D.GetXaxis().GetBinLowEdge(ibin)) + " < " +
				                label + " < " +
				                str(histogram_2D.GetXaxis().GetBinLowEdge(ibin+1)))
			yield slice

