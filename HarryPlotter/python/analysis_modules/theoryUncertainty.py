# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import hashlib
import math

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class TheoryUncertainty(analysisbase.AnalysisBase):
	"""Calculating theoretical uncertainties via acceptance + control plots"""

	def __init__(self):
		super(TheoryUncertainty, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(TheoryUncertainty, self).modify_argument_parser(parser, args)
		
		self.efficiency_options = parser.add_argument_group("Acceptance options")
		self.efficiency_options.add_argument("--numerator", nargs="+", default=[None], help="Nick names for the numerators of the acceptance.")
		self.efficiency_options.add_argument("--denominator", nargs="+", default=[None], help="Nick names for the denominator of the acceptance.")

	def prepare_args(self, parser, plotData):
		super(TheoryUncertainty, self).prepare_args(parser, plotData)
		
		plotData.plotdict["numerator"] = [nick for nick in plotData.plotdict["numerator"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["denominator"] = [nick for nick in plotData.plotdict["denominator"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["numerator", "denominator"])
		
	def run(self, plotData=None):
		super(TheoryUncertainty, self).run(plotData)
		A = {}
		central_value = ""
		for index, (numerator, denominator ) in enumerate(zip(plotData.plotdict["numerator"], plotData.plotdict["denominator"])):
			A[numerator] = plotData.plotdict["root_objects"][numerator].Integral() / plotData.plotdict["root_objects"][denominator].Integral() 
			#plotData.plotdict["root_objects"][numerator].Print()
		unc = (max(A.values()) - min(A.values())) / 2. / A[plotData.plotdict["centralvalue"]]
		print "Unc:", unc 