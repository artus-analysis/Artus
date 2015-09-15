# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import ROOT
import copy
import pprint
import Artus.HarryPlotter.analysisbase as analysisbase



class ProfileX(analysisbase.AnalysisBase):
	""" Convert 2D histogram into a TProfile. """

	def modify_argument_parser(self, parser, args):
		super(ProfileX, self).modify_argument_parser(parser, args)

		self.profileX_options = parser.add_argument_group("{} options".format(self.name()))
		self.profileX_options.add_argument("--profileX-nicks", type=str, nargs="+",
				help="Nick names for the Histograms to be Profiled. Default: all TH2 Histograms.")
		self.profileX_options.add_argument("--profileX-result-nicks", type=str, nargs="+",
				help="Nick names for the resulting TProfile.")

	def prepare_args(self, parser, plotData):
		super(ProfileX, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["profileX_nicks", "profileX_result_nicks"])

	def run(self, plotData=None):
		super(ProfileX, self).run(plotData)
		if(plotData.plotdict["profileX_nicks"] == [None]):
			old_nicks  = copy.deepcopy(plotData.plotdict["nicks"])
			new_nicks  = copy.deepcopy(plotData.plotdict["nicks"])
		else:
			old_nicks = plotData.plotdict["profileX_nicks"]
			new_nicks = plotData.plotdict["profileX_result_nicks"]
		for old_nick, new_nick in zip(old_nicks, new_nicks):
			histogram_2D = plotData.plotdict["root_objects"][old_nick]
			if isinstance(histogram_2D, ROOT.TH2):
				del plotData.plotdict["root_objects"][old_nick]
				plotData.plotdict["root_objects"][new_nick] = histogram_2D.ProfileX()
