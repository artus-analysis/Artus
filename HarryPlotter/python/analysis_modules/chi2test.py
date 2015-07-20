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

class Chi2Test(analysisbase.AnalysisBase):
	"""Chi2Test for comparing histograms"""

	def __init__(self):
		super(Chi2Test, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Chi2Test, self).modify_argument_parser(parser, args)
		
		self.chi2test_options = parser.add_argument_group("Chi2Test options")
		self.chi2test_options.add_argument(
				"--chi2test-nicks", nargs="+",
				help="Nick names (whitespace separated) for the two histograms to be compared"
		)
		self.chi2test_options.add_argument(
				"--chi2test-compare", nargs="+", default=["UU"],
				help="Specify the type of histograms to compare. See TH1::Chi2Test. [Default: %(default)s]"
		)

	def prepare_args(self, parser, plotData):
		super(Chi2Test, self).prepare_args(parser, plotData)

		self.prepare_list_args(plotData, ["chi2test_nicks", "chi2test_compare"])

		for index, (chi2test_nicks, chi2test_compare) in enumerate(zip(
				*[plotData.plotdict[k] for k in ["chi2test_nicks", "chi2test_compare"]]
		)):
			plotData.plotdict["chi2test_nicks"][index] = chi2test_nicks.split()
			warning = False
			if len(plotData.plotdict["chi2test_nicks"][index]) > 2:
				warning = True
				log.warning("More than 2 chi2test_nicks. The comparison has to be between 2 histograms")

	def run(self, plotData=None):
		super(Chi2Test, self).run(plotData)

		for chi2test_nicks, chi2test_compare in zip(
				*[plotData.plotdict[k] for k in ["chi2test_nicks", "chi2test_compare"]]
		):
			print "chi2test between ", chi2test_nicks[0], " and ", chi2test_nicks[1]
			chi2test_result = plotData.plotdict["root_objects"][chi2test_nicks[0]].Chi2Test(plotData.plotdict["root_objects"][chi2test_nicks[1]], chi2test_compare)
