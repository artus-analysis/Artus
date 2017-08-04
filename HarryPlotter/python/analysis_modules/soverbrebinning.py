# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import sys

import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools


class SOverBRebinning(analysisbase.AnalysisBase):
	"""Create s/b rebinned histograms including data points."""

	def modify_argument_parser(self, parser, args):
		super(SOverBRebinning, self).modify_argument_parser(parser, args)

		self.rebinning_options = parser.add_argument_group("{} options".format(self.name()))
		self.rebinning_options.add_argument(
				"--signal-nicks", type=str, nargs="+", default=[],
				help="Nick names (whitespace separated) for the signal histograms."
		)
		self.rebinning_options.add_argument(
				"--background-nicks", type=str, nargs="+", default=[],
				help="Nick names (whitespace separated) for the background histograms."
		)
		self.rebinning_options.add_argument(
				"--data-nicks", type=str, nargs="+", default=[],
				help="Nick names (whitespace separated) for the data histograms."
		)
		self.rebinning_options.add_argument(
				"--rebinned-binning", type=str, nargs="+", default=[]
		)
		self.rebinning_options.add_argument(
				"--rebinned-name", type=str, nargs="+", default=[]
		)

	def prepare_args(self, parser, plotData):
		super(SOverBRebinning, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["signal_nicks", "background_nicks", "data_nicks", "rebinned_binning", "rebinned_name"])

	def run(self, plotData=None):
		super(SOverBRebinning, self).run(plotData)
		for signal_nick, background_nick, data_nick, rebinning_binning, rebinned_name in zip(*[plotData.plotdict[k] for k in ["signal_nicks", "background_nicks", "data_nicks", "rebinned_binning", "rebinned_name"]]):
			binning = [float(a) for a in rebinning_binning.split(" ")]
			log.debug("rebinned_name: ", rebinned_name + "_s")

			signal_histo = roottools.RootTools.create_root_histogram(binning, name = rebinned_name + "_s")
			bkg_histo = roottools.RootTools.create_root_histogram(binning, name = rebinned_name + "_b")
			data_histo = roottools.RootTools.create_root_histogram(binning, name = rebinned_name + "_data")
			nbinsX = plotData.plotdict["root_objects"][signal_nick].GetNbinsX()
			log.debug( signal_nick, " / ", background_nick, " / ", data_nick )

			for xbin in range(1,nbinsX,1):
				signal = plotData.plotdict["root_objects"][signal_nick].GetBinContent(xbin)
				signal_e = plotData.plotdict["root_objects"][signal_nick].GetBinError(xbin)
				bkg = plotData.plotdict["root_objects"][background_nick].GetBinContent(xbin)
				bkg_e = plotData.plotdict["root_objects"][background_nick].GetBinError(xbin)
				data = plotData.plotdict["root_objects"][data_nick].GetBinContent(xbin)
				data_e = plotData.plotdict["root_objects"][data_nick].GetBinError(xbin)
				sb = signal / (signal+bkg)
				if sb < binning[0]:
					sb = binning[0]
				current_bin = signal_histo.FindBin(sb)

				signal_histo.SetBinContent(current_bin, signal+signal_histo.GetBinContent(current_bin))
				signal_histo.SetBinError(current_bin, signal_e+signal_histo.GetBinError(current_bin))

				bkg_histo.SetBinContent(current_bin, bkg+bkg_histo.GetBinContent(current_bin))
				bkg_histo.SetBinError(current_bin, bkg_e+bkg_histo.GetBinError(current_bin))

				data_histo.SetBinContent(current_bin, data+data_histo.GetBinContent(current_bin))
				data_histo.SetBinError(current_bin, data_e+data_histo.GetBinError(current_bin))

				if (sb > 0.1):
					print data_nick, " : ", sb, ", content: ", data, ", bin: ", xbin, ", signal:", signal, " bg: ", bkg
			
			plotData.plotdict["nicks"].append(rebinned_name + "_s")
			plotData.plotdict["nicks"].append(rebinned_name + "_b")
			plotData.plotdict["nicks"].append(rebinned_name + "_data")
			plotData.plotdict["root_objects"][rebinned_name + "_s"] = signal_histo
			plotData.plotdict["root_objects"][rebinned_name + "_b"] = bkg_histo
			plotData.plotdict["root_objects"][rebinned_name + "_data"] = data_histo


			name_hash = hashlib.md5("_".join([str(signal_nicks), str(background_nicks), str(rebinning_nicks)])).hexdigest()
			
			signal_histogram = None
			for signal_nick in signal_nicks:
				if signal_histogram is None:
					signal_histogram = plotData.plotdict["root_objects"][signal_nick].Clone("signal_"+name_hash)
				else:
					signal_histogram.Add(plotData.plotdict["root_objects"][signal_nick])
			
			background_histogram = None
			for background_nick in background_nicks:
				if background_histogram is None:
					background_histogram = plotData.plotdict["root_objects"][background_nick].Clone("background_"+name_hash)
				else:
					background_histogram.Add(plotData.plotdict["root_objects"][background_nick])
			
			s_over_b_histogram = signal_histogram.Clone("ratio_"+name_hash)
			s_over_b_histogram.Divide(background_histogram)
			
			s_over_b_ratios = []
			for x_bin in xrange(1, s_over_b_histogram.GetNbinsX()+1):
				for y_bin in xrange(1, s_over_b_histogram.GetNbinsY()+1):
					for z_bin in xrange(1, s_over_b_histogram.GetNbinsZ()+1):
						global_bin = s_over_b_histogram.GetBin(x_bin, y_bin, z_bin)
						s_over_b_ratios.append((global_bin, s_over_b_histogram.GetBinContent(global_bin)))
			s_over_b_ratios.sort(key=lambda item: item[1], reverse=False)
			
			for rebinning_nick in rebinning_nicks:
				histogram = plotData.plotdict["root_objects"][rebinning_nick]
				rebinned_histogram = ROOT.TH1D("histogram_"+name_hash, "", len(s_over_b_ratios), 0.0, float(len(s_over_b_ratios)))
				for index, global_bin in enumerate(zip(*s_over_b_ratios)[0]):
					rebinned_histogram.SetBinContent(index+1, histogram.GetBinContent(global_bin))
					rebinned_histogram.SetBinError(index+1, histogram.GetBinError(global_bin))
				plotData.plotdict["root_objects"][rebinning_nick] = rebinned_histogram

