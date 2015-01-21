# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import hashlib
import numpy

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class Efficiency(analysisbase.AnalysisBase):
	"""Build efficiency graphs from two histograms"""

	def __init__(self):
		super(Efficiency, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Efficiency, self).modify_argument_parser(parser, args)
		
		self.efficiency_options = parser.add_argument_group("Efficiency options")
		self.efficiency_options.add_argument("--efficiency-numerator-nicks", nargs="+", required=True,
				help="Nick names for the numerators of the efficiencies.")
		self.efficiency_options.add_argument("--efficiency-denominator-nicks", nargs="+", required=True,
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
		self.cut_efficiency_options.add_argument("--cut-efficiency-sig-nicks", nargs="+", required=True,
				help="Nick names for the signal histograms.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-bkg-nicks", nargs="+", required=True,
				help="Nick names for the background histograms.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-nicks", nargs="+", default=[None],
				help="Nick names for the resulting ROC graphs.")
		self.cut_efficiency_options.add_argument("--cut-efficiency-modes", nargs="+", default=["sigEffVsBkgRej"],
				choices=["sigEffVsBkgEff", "sigEffVsBkgRej", "sigRejVsBkgEff", "sigRejVsBkgRej"],
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
			
			sigCutEfficiency = CutEfficiencyCalculator(plotData.plotdict["root_objects"][cut_efficiency_sig_nick])
			bkgCutEfficiency = CutEfficiencyCalculator(plotData.plotdict["root_objects"][cut_efficiency_bkg_nick])
			
			cut_efficiency_graph = ROOT.TGraph(
				plotData.plotdict["root_objects"][cut_efficiency_sig_nick].GetNbinsX()+1,
				array.array("d", bkgCutEfficiency.get_cut_efficiencies(invertCut=("bkgrej" in cut_efficiency_mode.lower()))),
				array.array("d", sigCutEfficiency.get_cut_efficiencies(invertCut=("sigrej" in cut_efficiency_mode.lower())))
			)
			
			cut_efficiency_graph_name = "histogram_" + hashlib.md5("_".join([plotData.plotdict["root_objects"][cut_efficiency_sig_nick].GetName(),
			                                                                 plotData.plotdict["root_objects"][cut_efficiency_bkg_nick].GetName()])).hexdigest()
			cut_efficiency_graph.SetName(cut_efficiency_graph_name)
			cut_efficiency_graph.SetTitle("")
			plotData.plotdict["root_objects"][cut_efficiency_nick] = cut_efficiency_graph


class CutEfficiencyCalculator(object):
	def __init__(self, rootHistogram):
		""" Constructor taking a 1D ROOT histogram. Caches the efficiencies. """
		
		# prepare histogram
		self._histogram = copy.deepcopy(rootHistogram)
		if  self._histogram.Integral() > 0.0:
			self._histogram.Scale(1.0 / self._histogram.Integral())
		self._histogram.SetBinContent(0, 0.0)
		self._histogram.SetBinContent(self._histogram.GetNbinsX()+1, 0.0)
		
		# calculate efficiencies
		self._cut_efficiencies = self.get_cut_efficiencies(update=True)
	
	def get_cut_efficiencies(self, invertCut=False, scaleFactor=1.0, update=False):
		"""
		Calculate efficiencies for sliding cut thresholds (bin edges)
		
		invertCut = True: Select events above cut
		invertCut = False: Select events below cut
		
		scaleFactor for scaling the efficiencies
		
		update = True: calculate the efficiencies
		update = False: use cached efficiencies
		"""
		
		cut_efficiencies = None
		
		# use cache if requested
		if not update:
			cut_efficiencies = self._cut_efficiencies
		
		# determine efficiencies
		bin_contents = numpy.array([self._histogram.GetBinContent(xBin) for xBin in xrange(0, self._histogram.GetNbinsX()+1)])
		cut_efficiencies = numpy.array([efficiency if efficiency < 1.0 else 1.0 for efficiency in bin_contents.cumsum()])
		
		# invert direction of cut if requested
		if invertCut:
			cut_efficiencies = (1.0 - cut_efficiencies)
		
		if scaleFactor != 1.0:
			cut_efficiencies = (scaleFactor * cut_efficiencies)
		
		return cut_efficiencies
	
	def get_cut_values(self):
		""" Return cut thresholds (bin edges). """
		return [self._histogram.GetBinLowEdge(xBin) for xBin in xrange(1, self._histogram.GetNbinsX()+2)]

