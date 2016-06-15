# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class  Errorband(analysisbase.AnalysisBase):
	"""Create TGraphAsymerrors from three histograms"""

	def modify_argument_parser(self, parser, args):
		super(Errorband, self).modify_argument_parser(parser, args)

		self.errorband_options = parser.add_argument_group("{} options".format(self.name()))
		self.errorband_options.add_argument(
				"--errorband-histogram-nicks", nargs="+",
				help="Nick names (whitespace separated) of the central value and the two systematic shifts"
		)
		self.errorband_options.add_argument(
				"--errorband-result-nicks", nargs="+",
				help="Result nicknames"
		)

	def prepare_args(self, parser, plotData):
		super(Errorband, self).prepare_args(parser, plotData)
		
		self.nick_triples = []
		self.result_nicks = []
		for nicks, result in zip(plotData.plotdict["errorband_histogram_nicks"], plotData.plotdict["errorband_result_nicks"]):
			self.nick_triples.append(nicks.split(" "))
			self.result_nicks.append(result)
			if( len(self.nick_triples[-1]) != 3):
				log.fatal("--errorband-histogram-nicks could not be parsed to three nicks")

	def run(self, plotData=None):
		super(Errorband, self).run(plotData)

		for triple, result_nick in zip(self.nick_triples, self.result_nicks):
			central = plotData.plotdict["root_objects"][triple[0]]
			shift_up  = plotData.plotdict["root_objects"][triple[1]]
			shift_down  = plotData.plotdict["root_objects"][triple[2]]

			root_object = ROOT.TGraphAsymmErrors(central.GetNbinsX())
			for bin_i in range(1,central.GetNbinsX()+1):
				root_object.SetPoint(bin_i, central.GetBinCenter(bin_i), central.GetBinContent(bin_i))
				root_object.SetPointEYhigh(bin_i, central.GetBinContent(bin_i) - shift_up.GetBinContent(bin_i))
				root_object.SetPointEYlow(bin_i, central.GetBinContent(bin_i) - shift_down.GetBinContent(bin_i))
				root_object.SetPointEXlow(bin_i, central.GetBinWidth(bin_i) / 2.0)
				root_object.SetPointEXhigh(bin_i, central.GetBinWidth(bin_i) / 2.0)
			plotData.plotdict["root_objects"][result_nick] = root_object
			plotData.plotdict["nicks"].append(result_nick)
