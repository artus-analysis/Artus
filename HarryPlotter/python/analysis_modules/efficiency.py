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
		self.cut_efficiency_options.add_argument(
				"--cut-efficiency-sig-nicks", nargs="+", default=[None],
				help="Nick names for the signal histograms."
		)
		self.cut_efficiency_options.add_argument(
				"--cut-efficiency-bkg-nicks", nargs="+", default=[None],
				help="Nick names for the background histograms."
		)
		self.cut_efficiency_options.add_argument(
				"--cut-efficiency-nicks", nargs="+", default=[None],
				help="Nick names for the resulting ROC graphs."
		)
		self.cut_efficiency_options.add_argument(
				"--select-lower-values", nargs="+", type="bool", default=[False],
				help="Select events lower than the cut. [Default: %(default)s]"
		)
		self.cut_efficiency_options.add_argument(
				"--cut-efficiency-modes", nargs="+", default=["sigEffVsBkgRej"],
				choices=["sigEff", "sigRej", "bkgEff", "bkgRej", "sigEffVsBkgEff", "sigEffVsBkgRej", "sigRejVsBkgEff", "sigRejVsBkgRej", "sigPur", "sOverSqrtSB"],
				help="ROC modes. [Default: %(default)s]"
		)

	def prepare_args(self, parser, plotData):
		super(CutEfficiency, self).prepare_args(parser, plotData)
		
		plotData.plotdict["cut_efficiency_sig_nicks"] = [nick for nick in plotData.plotdict["cut_efficiency_sig_nicks"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["cut_efficiency_bkg_nicks"] = [nick for nick in plotData.plotdict["cut_efficiency_bkg_nicks"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["cut_efficiency_sig_nicks", "cut_efficiency_bkg_nicks", "cut_efficiency_nicks", "select_lower_values", "cut_efficiency_modes"])
		
		for index, nick in enumerate(plotData.plotdict["cut_efficiency_nicks"]):
			if not nick:
				nick = plotData.plotdict["cut_efficiency_sig_nicks"][index] + "_" + plotData.plotdict["cut_efficiency_bkg_nicks"][index]
				plotData.plotdict["cut_efficiency_nicks"][index] = nick
			if not nick in plotData.plotdict["nicks"]:
				plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(plotData.plotdict["cut_efficiency_bkg_nicks"][index])+1, nick)

	def run(self, plotData=None):
		super(CutEfficiency, self).run(plotData)
		
		for index, (cut_efficiency_sig_nick, cut_efficiency_bkg_nick, cut_efficiency_nick, select_lower_values, cut_efficiency_mode) in enumerate(zip(
				plotData.plotdict["cut_efficiency_sig_nicks"],
				plotData.plotdict["cut_efficiency_bkg_nicks"],
				plotData.plotdict["cut_efficiency_nicks"],
				plotData.plotdict["select_lower_values"],
				plotData.plotdict["cut_efficiency_modes"]
		)):
			name_hash = hashlib.md5("_".join([cut_efficiency_sig_nick, cut_efficiency_bkg_nick, cut_efficiency_nick, str(select_lower_values), cut_efficiency_mode])).hexdigest()
			
			signal_histogram = plotData.plotdict["root_objects"][cut_efficiency_sig_nick]
			background_histogram = plotData.plotdict["root_objects"][cut_efficiency_bkg_nick]
			assert isinstance(signal_histogram, ROOT.TH1)
			assert isinstance(background_histogram, ROOT.TH1)
			
			cut_efficiency_mode = cut_efficiency_mode.lower()
			
			signal_cumulative = CutEfficiency.get_cumulative_histogram(signal_histogram, select_lower_values, "sigcum_"+name_hash)
			background_cumulative = CutEfficiency.get_cumulative_histogram(background_histogram, select_lower_values, "bkgcum_"+name_hash)
			
			if cut_efficiency_mode == "sigpur":
				total_cumulative = background_cumulative.Clone("totcum_"+name_hash)
				total_cumulative.Add(signal_cumulative)
				
				signal_purity = ROOT.TGraphAsymmErrors(signal_cumulative, total_cumulative)
				graph = signal_purity
			
			elif cut_efficiency_mode == "soversqrtsb":
				assert signal_cumulative.GetNbinsX() == background_cumulative.GetNbinsX()
				assert signal_cumulative.GetNbinsY() == background_cumulative.GetNbinsY()
				assert signal_cumulative.GetNbinsZ() == background_cumulative.GetNbinsZ()
				
				result_histogram = signal_cumulative.Clone("soversqrtsb"+name_hash)
				for x_bin in range(1, signal_cumulative.GetNbinsX()+1):
					for y_bin in range(1, signal_cumulative.GetNbinsY()+1):
						for z_bin in range(1, signal_cumulative.GetNbinsY()+1):
							global_bin = signal_cumulative.GetBin(x_bin, y_bin, z_bin)
							s, se = signal_cumulative.GetBinContent(global_bin), signal_cumulative.GetBinError(global_bin)
							b, be = background_cumulative.GetBinContent(global_bin), background_cumulative.GetBinError(global_bin)
							
							bin_content = (s / math.sqrt(s+b)) if s+b != 0.0 else 0.0
							# https://www.wolframalpha.com/input/?i=Sqrt%5B(x*D%5Bs%2FSqrt%5Bs%2Bb%5D,s%5D)%5E2%2B(y*D%5Bs%2FSqrt%5Bs%2Bb%5D,b%5D)%5E2%5D
							bin_error = (math.sqrt((4.0*b*b*se*se + 4.0*b*s*se*se + s*s*se*se + s*s*be*be)/((b+s)*(b+s)*(b+s)))/2.0) if s+b != 0.0 else 0.0
							
							result_histogram.SetBinContent(global_bin, bin_content)
							result_histogram.SetBinError(global_bin, bin_error)
				graph = result_histogram
			
			else:
				total_signal = CutEfficiency.get_total_from_cumulative(signal_cumulative, "totsig_"+name_hash)
				total_background = CutEfficiency.get_total_from_cumulative(background_cumulative, "totbkg_"+name_hash)
				
				signal_efficiency = ROOT.TGraphAsymmErrors(signal_cumulative, total_signal)
				background_efficiency = ROOT.TGraphAsymmErrors(background_cumulative, total_background)
			
				if (cut_efficiency_mode == "sigeff") or (cut_efficiency_mode == "sigrej"):
					graph = CutEfficiency.build_graph(
							signal_efficiency, signal_efficiency,
							x_take_x=True,
							y_modifier=lambda y: (1.0-y) if "rej" in cut_efficiency_mode else y
					)
				
				elif (cut_efficiency_mode == "bkgeff") or (cut_efficiency_mode == "bkgrej"):
					graph = CutEfficiency.build_graph(
							background_efficiency, background_efficiency,
							x_take_x=True,
							y_modifier=lambda y: (1.0-y) if "rej" in cut_efficiency_mode else y
					)
				
				else:
					graph = CutEfficiency.build_graph(
							background_efficiency, signal_efficiency,
							x_modifier=lambda x: (1.0-x) if "bkgrej" in cut_efficiency_mode else x,
							y_modifier=lambda y: (1.0-y) if "sigrej" in cut_efficiency_mode else y
					)
			
			graph.SetName("efficiency_"+name_hash)
			graph.SetTitle("")
			plotData.plotdict["root_objects"][cut_efficiency_nick] = graph
	
	@staticmethod
	def get_cumulative_histogram(histogram, forward=True, name="cumulative"):
		# https://root.cern.ch/root/html/src/TH1.cxx.html#FN_D.D
		cumulative = histogram.Clone(name)
		sum_content = 0.0
		sum_error = 0.0
		for x_bin in range(1, cumulative.GetNbinsX()+1)[::(1 if forward else -1)]:
			for y_bin in range(1, cumulative.GetNbinsY()+1)[::(1 if forward else -1)]:
				for z_bin in range(1, cumulative.GetNbinsY()+1)[::(1 if forward else -1)]:
					src_global_bin = histogram.GetBin(x_bin, y_bin, z_bin)
					#dst_global_bin = cumulative.GetBin(
					#		abs((0 if forward else (cumulative.GetNbinsX()+1)) - x_bin),
					#		abs((0 if forward else (cumulative.GetNbinsY()+1)) - y_bin),
					#		abs((0 if forward else (cumulative.GetNbinsZ()+1)) - z_bin),
					#)
					sum_content += histogram.GetBinContent(src_global_bin)
					sum_error = math.sqrt((sum_error*sum_error) + (histogram.GetBinError(src_global_bin)*histogram.GetBinError(src_global_bin)))
					cumulative.SetBinContent(src_global_bin, sum_content)
					cumulative.SetBinError(src_global_bin, sum_error)
		global_overflow = histogram.GetBin( cumulative.GetNbinsX()+1, cumulative.GetNbinsY()+1,cumulative.GetNbinsZ()+1)
		cumulative.SetBinContent(0, 0)
		cumulative.SetBinContent(global_overflow, 0)
		return cumulative
	
	@staticmethod
	def get_total_from_cumulative(cumulative, name):
		total = cumulative.Clone(name)
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
	def build_graph(x_graph, y_graph, x_take_x=False, y_take_x=False, x_modifier=lambda x: x, y_modifier=lambda y: y):
		assert isinstance(x_graph, ROOT.TGraphAsymmErrors)
		assert isinstance(y_graph, ROOT.TGraphAsymmErrors)
		
		x_values = x_graph.GetX() if x_take_x else x_graph.GetY()
		x_values = array.array("d", [x_modifier(x_values[i]) for i in xrange(x_graph.GetN())])
		
		x_errors_low = x_graph.GetEXlow() if x_take_x else x_graph.GetEYlow()
		x_errors_low = array.array("d", [x_errors_low[i] for i in xrange(x_graph.GetN())])
		
		x_errors_high = x_graph.GetEXhigh() if x_take_x else x_graph.GetEYhigh()
		x_errors_high = array.array("d", [x_errors_high[i] for i in xrange(x_graph.GetN())])
		
		y_values = y_graph.GetX() if y_take_x else y_graph.GetY()
		y_values = array.array("d", [y_modifier(y_values[i]) for i in xrange(y_graph.GetN())])
		
		y_errors_low = y_graph.GetEXlow() if y_take_x else y_graph.GetEYlow()
		y_errors_low = array.array("d", [y_errors_low[i] for i in xrange(y_graph.GetN())])
		
		y_errors_high = y_graph.GetEXhigh() if y_take_x else y_graph.GetEYhigh()
		y_errors_high = array.array("d", [y_errors_high[i] for i in xrange(y_graph.GetN())])
		
		graph = ROOT.TGraphAsymmErrors(
				x_graph.GetN(),
				x_values, y_values,
				x_errors_low, x_errors_high,
				y_errors_low, y_errors_high
		)
		return graph

