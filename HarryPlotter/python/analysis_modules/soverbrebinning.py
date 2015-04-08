# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class SOverBRebinning(analysisbase.AnalysisBase):
	"""Reorder bins of 1-3D. histograms according to the signal over background ratio, resulting in 1D histograms. All input histograms need to have the same binning."""

	def modify_argument_parser(self, parser, args):
		super(SOverBRebinning, self).modify_argument_parser(parser, args)

		self.rebinning_options = parser.add_argument_group("{} options".format(self.name()))
		self.rebinning_options.add_argument(
				"--signal-nicks", type=str, nargs="+", default=[],
				help="Nick names for the signal histograms."
		)
		self.rebinning_options.add_argument(
				"--background-nicks", type=str, nargs="+", default=[],
				help="Nick names for the background histograms."
		)
		self.rebinning_options.add_argument(
				"--rebinning-nicks", type=str, nargs="+", default=[],
				help="Nick names (whitespace separated) for the histograms to be rebinned."
		)

	def prepare_args(self, parser, plotData):
		super(SOverBRebinning, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["signal_nicks", "background_nicks", "rebinning_nicks"])
		plotData.plotdict["rebinning_nicks"] = [nicks.split() for nicks in plotData.plotdict["divide_result_nicks"]]

	def run(self, plotData=None):
		super(SOverBRebinning, self).run(plotData)

		for signal_nick, background_nick, rebinning_nicks in zip([plotData.plotdict[k] for k in ["signal_nicks", "background_nicks", "rebinning_nicks"]]):
			name_hash = hashlib.md5("_".join([signal_nick, background_nick, str(rebinning_nicks)])).hexdigest()
			
			s_over_b_histogram = plotData.plotdict["root_objects"][signal_nick].Clone("signal_"+name_hash)
			s_over_b_histogram.Divide(plotData.plotdict["root_objects"][background_nick])
			
			s_over_b_ratios = []
			for x_bin in xrange(1, s_over_b_histogram.GetNbinsX()+1):
				for y_bin in xrange(1, s_over_b_histogram.GetNbinsY()+1):
					for z_bin in xrange(1, s_over_b_histogram.GetNbinsZ()+1):
						global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
						s_over_b_ratios.append((global_bin, s_over_b_histogram.GetBinContent(global_bin)))
			s_over_b_ratios.sort(key=lambda item: item[1], reverse=False)
			
			for rebinning_nick in rebinning_nicks:
				histogram = plotData.plotdict["root_objects"][rebinning_nick]
				rebinned_histogram = ROOT.TH1D("histogram_"+name_hash, "", len(s_over_b_ratios), 0.0, float(len(s_over_b_ratios)))
				for index, global_bin in enumerate(zip(*s_over_b_ratios)[0]):
					rebinned_histogram.SetBinContent(index+1, histogram.GetBinContent(global_bin))
					rebinned_histogram.SetBinError(index+1, histogram.GetBinError(global_bin))
				plotData.plotdict["root_objects"][rebinning_nick] = rebinned_histogram

