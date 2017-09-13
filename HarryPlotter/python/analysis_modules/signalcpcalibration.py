# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysisbase as analysisbase

import hashlib
import ROOT

class SignalCPCalibration(analysisbase.AnalysisBase):
	"""This module applies the calibration procedure via DY events of the phi*cp distribution of the signal, as suggested by Berge et al. in arxiv:1408.0798."""
	def __init__(self):
		super(SignalCPCalibration, self).__init__()


	def modify_argument_parser(self, parser, args):
		super(SignalCPCalibration, self).modify_argument_parser(parser, args)

		self.signal_calibration_options = parser.add_argument_group("Signal CP calibration options")
		self.signal_calibration_options.add_argument("--higgscp-nicks", nargs="+",
				help="Nicknames of the higgs signal samples.")
		self.signal_calibration_options.add_argument("--dy-nicks", nargs="+",
				help="Nicknames of the dy samples.")
		self.signal_calibration_options.add_argument("--calibrated-nicks", nargs="+", default=[None],
				help="Nicknames for the resulting histograms.")


	def prepare_args(self, parser, plotData):
		super(SignalCPCalibration, self).prepare_args(parser, plotData)
		
		plotData.plotdict["higgscp_nicks"] = [nick for nick in plotData.plotdict["higgscp_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["dy_nicks"] = [nick for nick in plotData.plotdict["dy_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["higgscp_nicks", "dy_nicks", "calibrated_nicks"])
		
		for index, nick in enumerate(plotData.plotdict["calibrated_nicks"]):
			if not nick:
				nick = plotData.plotdict["higgscp_nicks"][index] + "_" + plotData.plotdict["dy_nicks"][index]
				plotData.plotdict["calibrated_nicks"][index] = nick


	def run(self, plotData=None):
		super(SignalCPCalibration, self).run(plotData)
		
		for index, (higgscp_nick, dy_nick, calibrated_nick) in enumerate(zip(plotData.plotdict["higgscp_nicks"],
		                                                                       plotData.plotdict["dy_nicks"],
		                                                                       plotData.plotdict["calibrated_nicks"])):
			
			signal_histogram_name = "higgscp_histogram_" + hashlib.md5("_".join([higgscp_nick, dy_nick, calibrated_nick])).hexdigest()
			dy_histogram_name = "dy_histogram_" + hashlib.md5("_".join([higgscp_nick, dy_nick, calibrated_nick])).hexdigest()
			new_histogram_name = "calibrated_histogram_" + hashlib.md5("_".join([higgscp_nick, dy_nick, calibrated_nick])).hexdigest()
	
			print signal_histogram_name, dy_histogram_name, new_histogram_name

			signal_histogram = plotData.plotdict["root_objects"][higgscp_nick].Clone(signal_histogram_name)
			dy_histogram = plotData.plotdict["root_objects"][dy_nick].Clone(dy_histogram_name)
			new_histogram = plotData.plotdict["root_objects"][higgscp_nick].Clone(new_histogram_name)


			if dy_histogram.Integral() != 0.0:
				dy_histogram.Scale(signal_histogram.Integral() / dy_histogram.Integral())
				x = signal_histogram.GetMinimum() + (signal_histogram.GetMaximum() - signal_histogram.GetMinimum())/2
				new_histogram.Add(dy_histogram, -1)
				for nbin in range(1, new_histogram.GetNbinsX()+1):
					new_histogram.AddBinContent(nbin, x)

			plotData.plotdict["root_objects"][calibrated_nick] = new_histogram
			
			# insert new nick between the two existing ones
			if calibrated_nick not in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(max(plotData.plotdict["nicks"].index(higgscp_nick), plotData.plotdict["nicks"].index(dy_nick)),
				                                  calibrated_nick)



