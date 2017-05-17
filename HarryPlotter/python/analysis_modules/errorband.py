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
		self.errorband_options.add_argument(
				"--errorband-percentage-error", nargs="+", type="bool", default=[False],
				help="Use relative errors (in percent) instead of absolute bin contents"
		)

	def prepare_args(self, parser, plotData):
		super(Errorband, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["errorband_histogram_nicks", "errorband_result_nicks", "errorband_percentage_error"])
		self.nick_triples = []
		self.result_nicks = []
		self.percentage_errors = []
		for nicks, result, percent in zip(plotData.plotdict["errorband_histogram_nicks"], plotData.plotdict["errorband_result_nicks"], plotData.plotdict["errorband_percentage_error"]):
			self.nick_triples.append(nicks.split(" "))
			self.result_nicks.append(result)
			self.percentage_errors.append(percent)
			if( len(self.nick_triples[-1]) != 3):
				log.fatal("--errorband-histogram-nicks could not be parsed to three nicks")

	def run(self, plotData=None):
		super(Errorband, self).run(plotData)

		for triple, result_nick, percent in zip(self.nick_triples, self.result_nicks, self.percentage_errors):
			central = plotData.plotdict["root_objects"][triple[0]]
			shift_up  = plotData.plotdict["root_objects"][triple[1]]
			shift_down  = plotData.plotdict["root_objects"][triple[2]]

			root_object = ROOT.TGraphAsymmErrors(central.GetNbinsX())
			for bin_i in range(1,central.GetNbinsX()+1):
				root_object.SetPoint(bin_i-1, central.GetBinCenter(bin_i), central.GetBinContent(bin_i))
				root_object.SetPointEXlow(bin_i-1, central.GetBinWidth(bin_i) / 2.0)
				root_object.SetPointEXhigh(bin_i-1, central.GetBinWidth(bin_i) / 2.0)
				if(percent):
					root_object.SetPointEYhigh(bin_i-1, central.GetBinContent(bin_i)*shift_up.GetBinContent(bin_i)/100.)
					root_object.SetPointEYlow(bin_i-1, central.GetBinContent(bin_i)*shift_down.GetBinContent(bin_i)/100.)
				else:
					root_object.SetPointEYhigh(bin_i-1, central.GetBinContent(bin_i) - shift_up.GetBinContent(bin_i))
					root_object.SetPointEYlow(bin_i-1, central.GetBinContent(bin_i) - shift_down.GetBinContent(bin_i))
			plotData.plotdict["root_objects"][result_nick] = root_object
			plotData.plotdict["nicks"].append(result_nick)

