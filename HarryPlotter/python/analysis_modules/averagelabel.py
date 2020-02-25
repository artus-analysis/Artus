# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class AverageLabel(analysisbase.AnalysisBase):
	"""Calculates the mean of a histogram and appends it to the labels.
	Short labels are recommended for readability"""
	def __init__(self):
		super(AverageLabel, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(AverageLabel, self).modify_argument_parser(parser, args)

	def prepare_args(self, parser, plotData):
		super(AverageLabel, self).prepare_args(parser, plotData)

	def run(self, plotData=None):
		super(AverageLabel, self).run(plotData)

		labeldict = {}
		meandict = {}
		i=0
		for nick in plotData.plotdict["nicks"]:
 			root_object = plotData.plotdict["root_objects"][nick]
			if log.isEnabledFor(logging.DEBUG):
				log.info("\nInfos for object with nick \"{nick}\":".format(nick=nick))
				root_object.Print("range")
				print("The average of the Histogram is %f" % (root_object.GetMean()))
			else:
				meandict[nick] = root_object.GetMean()
				labeldict[nick] = plotData.plotdict["labels"][i]
				plotData.plotdict["labels"][i] += ", #bar{x} = %.1e" % (root_object.GetMean())
				i+=1
