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


class Efficiency(analysisbase.AnalysisBase):
	"""Build efficiency graphs from two histograms"""

	def __init__(self):
		super(Efficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Efficiency, self).modify_argument_parser(parser, args)
		
		self.efficiency_options = parser.add_argument_group("Efficiency options")
		self.efficiency_options.add_argument("--efficiency-numerator-nicks", nargs="+", default=[None],
				help="Nick names for the numerators of the efficiencies.")
		self.efficiency_options.add_argument("--efficiency-denominator-nicks", nargs="+", default=[None],
				help="Nick names for the denominator of the efficiencies.")
		self.efficiency_options.add_argument("--efficiency-nicks", nargs="+", default=[None],
				help="Nick names for the resulting efficiency graphs.")
		self.efficiency_options.add_argument("--efficiency-methods", nargs="+", default=["cp"],
				help="(Error) calculation methods. See TGraphAsymmErrors.Divide for further information. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(Efficiency, self).prepare_args(parser, plotData)
		
		plotData.plotdict["efficiency_numerator_nicks"] = [nick for nick in plotData.plotdict["efficiency_numerator_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["efficiency_denominator_nicks"] = [nick for nick in plotData.plotdict["efficiency_denominator_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["efficiency_numerator_nicks", "efficiency_denominator_nicks", "efficiency_nicks", "efficiency_methods"])
		
		for index, nick in enumerate(plotData.plotdict["efficiency_nicks"]):
			if not nick:
				nick = plotData.plotdict["efficiency_numerator_nicks"][index] + "_" + plotData.plotdict["efficiency_denominator_nicks"][index]
				plotData.plotdict["efficiency_nicks"][index] = nick
			if not nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(plotData.plotdict["efficiency_denominator_nicks"][index])+1, nick)

	def run(self, plotData=None):
		super(Efficiency, self).run(plotData)
		
		for index, (efficiency_numerator_nick, efficiency_denominator_nick, efficiency_nick, efficiency_method) in enumerate(zip(
				plotData.plotdict["efficiency_numerator_nicks"],
				plotData.plotdict["efficiency_denominator_nicks"],
				plotData.plotdict["efficiency_nicks"],
				plotData.plotdict["efficiency_methods"]
		)):
			
			efficiency_graph_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_objects"][efficiency_numerator_nick].GetName(),
			                                                             plotData.plotdict["root_objects"][efficiency_denominator_nick].GetName()])).hexdigest()
			
			efficiency_graph = ROOT.TGraphAsymmErrors(
					plotData.plotdict["root_objects"][efficiency_numerator_nick],
					plotData.plotdict["root_objects"][efficiency_denominator_nick]
			)
			efficiency_graph.SetName(efficiency_graph_name)
			efficiency_graph.SetTitle("")
			plotData.plotdict["root_objects"][efficiency_nick] = efficiency_graph



class CutEfficiency(analysisbase.AnalysisBase):
	"""Build ROC graphs from two histograms"""

	def __init__(self):
		super(CutEfficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(CutEfficiency, self).modify_argument_parser(parser, args)
		
		self.cut_efficiency_options = parser.add_argument_group("Cut efficiency options")
		self.cut_efficiency_options.add_argument("--cut-efficiency-sig-nicks", nargs="+", default=[None],
				help="Nick names for the signal histograms.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-bkg-nicks", nargs="+", default=[None],
				help="Nick names for the background histograms.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-nicks", nargs="+", default=[None],
				help="Nick names for the resulting ROC graphs.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-modes", nargs="+", default=["sigEffVsBkgRej"],
				choices=["sigEff", "sigRej", "bkgEff", "bkgRej", "sigEffVsBkgEff", "sigEffVsBkgRej", "sigRejVsBkgEff", "sigRejVsBkgRej"],
				help="ROC modes. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(CutEfficiency, self).prepare_args(parser, plotData)
		
		plotData.plotdict["cut_efficiency_sig_nicks"] = [nick for nick in plotData.plotdict["cut_efficiency_sig_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["cut_efficiency_bkg_nicks"] = [nick for nick in plotData.plotdict["cut_efficiency_bkg_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["cut_efficiency_sig_nicks", "cut_efficiency_bkg_nicks", "cut_efficiency_nicks", "cut_efficiency_modes"])
		
		for index, nick in enumerate(plotData.plotdict["cut_efficiency_nicks"]):
			if not nick:
				nick = plotData.plotdict["cut_efficiency_sig_nicks"][index] + "_" + plotData.plotdict["cut_efficiency_bkg_nicks"][index]
				plotData.plotdict["cut_efficiency_nicks"][index] = nick
			if not nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(plotData.plotdict["cut_efficiency_bkg_nicks"][index])+1, nick)

	def run(self, plotData=None):
		super(CutEfficiency, self).run(plotData)
		
		for index, (cut_efficiency_sig_nick, cut_efficiency_bkg_nick, cut_efficiency_nick, cut_efficiency_mode) in enumerate(zip(
				plotData.plotdict["cut_efficiency_sig_nicks"],
				plotData.plotdict["cut_efficiency_bkg_nicks"],
				plotData.plotdict["cut_efficiency_nicks"],
				plotData.plotdict["cut_efficiency_modes"]
		)):
			signal_histogram = plotData.plotdict["root_objects"][cut_efficiency_sig_nick]
			background_histogram = plotData.plotdict["root_objects"][cut_efficiency_bkg_nick]
			assert isinstance(signal_histogram, ROOT.TH1)
			assert isinstance(background_histogram, ROOT.TH1)
			
			cut_efficiency_mode = cut_efficiency_mode.lower()
			
			signal_cumulative = CutEfficiency.get_cumulative_histogram(signal_histogram)
			background_cumulative = CutEfficiency.get_cumulative_histogram(background_histogram)
			
			total_signal = CutEfficiency.get_total_from_cumulative(signal_cumulative)
			total_background = CutEfficiency.get_total_from_cumulative(background_cumulative)
			
			signal_efficiency = ROOT.TGraphAsymmErrors(signal_cumulative, total_signal)
			background_efficiency = ROOT.TGraphAsymmErrors(background_cumulative, total_background)
			
			if (cut_efficiency_mode == "sigeff") or (cut_efficiency_mode == "sigrej"):
				efficiency = signal_efficiency
			elif (cut_efficiency_mode == "bkgeff") or (cut_efficiency_mode == "bkgrej"):
				efficiency = background_efficiency
			else:
				efficiency = CutEfficiency.build_graph(background_efficiency, signal_efficiency, "bkgrej" in cut_efficiency_mode, "sigrej" in cut_efficiency_mode)
			
			efficiency_name = "histogram_" + hashlib.md5("_".join([signal_histogram.GetName(), background_histogram.GetName()])).hexdigest()
			efficiency.SetName(efficiency_name)
			efficiency.SetTitle("")
			plotData.plotdict["root_objects"][cut_efficiency_nick] = efficiency
	
	@staticmethod
	def get_cumulative_histogram(histogram, forward=True):
		# https://root.cern.ch/root/html/src/TH1.cxx.html#FN_D.D
		cumulative = histogram.Clone(histogram.GetName()+"_cumulative")
		sum_content = 0.0
		sum_error = 0.0
		for x_bin in range(1, cumulative.GetNbinsX()+1)[::(1 if forward else -1)]:
			for y_bin in range(1, cumulative.GetNbinsY()+1)[::(1 if forward else -1)]:
				for z_bin in range(1, cumulative.GetNbinsY()+1)[::(1 if forward else -1)]:
					src_global_bin = histogram.GetBin(x_bin, y_bin, z_bin)
					dst_global_bin = cumulative.GetBin(
							abs((0 if forward else (cumulative.GetNbinsX()+1)) - x_bin),
							abs((0 if forward else (cumulative.GetNbinsY()+1)) - y_bin),
							abs((0 if forward else (cumulative.GetNbinsZ()+1)) - z_bin),
					)
					sum_content += histogram.GetBinContent(src_global_bin)
					sum_error = math.sqrt((sum_error*sum_error) + (histogram.GetBinError(src_global_bin)*histogram.GetBinError(src_global_bin)))
					cumulative.SetBinContent(dst_global_bin, sum_content)
					cumulative.SetBinError(dst_global_bin, sum_error)
		return cumulative
	
	@staticmethod
	def get_total_from_cumulative(cumulative):
		total = cumulative.Clone(cumulative.GetName()+"_total")
		total.Reset()
		max_bin_content = cumulative.GetBinContent(cumulative.GetMaximumBin())
		max_bin_error = cumulative.GetBinError(cumulative.GetMaximumBin())
		for x_bin in xrange(1, total.GetNbinsX()+1):
			for y_bin in xrange(1, total.GetNbinsY()+1):
				for z_bin in xrange(1, total.GetNbinsY()+1):
					global_bin = total.GetBin(x_bin, y_bin, z_bin)
					total.SetBinContent(global_bin, max_bin_content)
					total.SetBinError(global_bin, max_bin_error)
		return total
	
	@staticmethod
	def build_graph(x_graph, y_graph, reverse_x=False, reverse_y=False):
		assert isinstance(x_graph, ROOT.TGraphAsymmErrors)
		assert isinstance(y_graph, ROOT.TGraphAsymmErrors)
		
		x_values = x_graph.GetY()
		x_values = array.array("d", [x_values[i] for i in range(x_graph.GetN())[::(-1 if reverse_x else 1)]])
		
		x_errors_low = x_graph.GetEYlow()
		x_errors_low = array.array("d", [x_errors_low[i] for i in range(x_graph.GetN())[::(-1 if reverse_x else 1)]])
		
		x_errors_high = x_graph.GetEYhigh()
		x_errors_high = array.array("d", [x_errors_high[i] for i in range(x_graph.GetN())[::(-1 if reverse_x else 1)]])
		
		y_values = x_graph.GetY()
		y_values = array.array("d", [y_values[i] for i in range(y_graph.GetN())[::(-1 if reverse_y else 1)]])
		
		y_errors_low = y_graph.GetEYlow()
		y_errors_low = array.array("d", [y_errors_low[i] for i in range(y_graph.GetN())[::(-1 if reverse_y else 1)]])
		
		y_errors_high = y_graph.GetEYhigh()
		y_errors_high = array.array("d", [y_errors_high[i] for i in range(y_graph.GetN())[::(-1 if reverse_y else 1)]])
		
		graph = ROOT.TGraphAsymmErrors(
				x_graph.GetN(),
				x_values, y_values,
				x_errors_low, x_errors_high,
				y_errors_low, y_errors_high
		)
		graph.SetName(y_graph.GetName()+"_vs_"+x_graph.GetName())
		graph.SetTitle("")
		return graph

