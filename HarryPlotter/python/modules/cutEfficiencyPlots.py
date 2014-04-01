
import array
import copy
import numpy
import pprint
import ROOT

import Artus.HarryPlotter.plot1d as plot1d
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.labels as labels


class CutEfficiency(object):
	def __init__(self, rootHistogram):
		""" Constructor taking a 1D ROOT histogram. Caches the efficiencies. """
		
		# prepare histogram
		self._histogram = copy.deepcopy(rootHistogram)
		self._histogram.Scale(1.0 / self._histogram.Integral())
		self._histogram.SetBinContent(0, 0.0)
		self._histogram.SetBinContent(self._histogram.GetNbinsX()+1, 0.0)
		
		# calculate efficiencies
		self._cut_efficiencies = self.get_cut_efficiencies(update=True)
	
	def get_cut_efficiencies(self, invertCut=False, update=False):
		"""
		Calculate efficiencies for sliding cut thresholds (bin edges)
		
		invertCut = True: Select events above cut
		invertCut = False: Select events below cut
		
		update = True: calculate the efficiencies
		update = False: use cached efficiencies
		"""
		
		cut_efficiencies = None
		
		# use cache if requested
		if not update:
			cut_efficiencies = self._cut_efficiencies
		
		# determine efficiencies
		binContents = numpy.array([self._histogram.GetBinContent(xBin) for xBin in xrange(0, self._histogram.GetNbinsX()+1)])
		cut_efficiencies = numpy.array([efficiency if efficiency < 1.0 else 1.0 for efficiency in binContents.cumsum()])
		
		# invert direction of cut if requested
		if invertCut:
			cut_efficiencies = (1.0 - self._cut_efficiencies)
		
		return cut_efficiencies
	
	def get_cut_values(self):
		""" Return cut thresholds (bin edges). """
		return [self._histogram.GetBinLowEdge(xBin) for xBin in xrange(1, self._histogram.GetNbinsX()+2)]


def cutEffPlot(plotdict, invertCut=False, yName="Efficiency"):
	""" Plot cut efficiencies for 1D histograms. """
	
	plot1d.get_root_histos(plotdict)
	
	# replace ROOT histograms by efficiency graphs
	for index, roothisto in enumerate(plotdict["roothistos"]):
		cutEfficiency = CutEfficiency(roothisto)
		efficiencyGraph = ROOT.TGraph(roothisto.GetNbinsX()+1,
		                              array.array("d", cutEfficiency.get_cut_values()),
		                              array.array("d", cutEfficiency.get_cut_efficiencies(invertCut=invertCut)))
		plotdict["roothistos"][index] = efficiencyGraph
	
	plotdict["yname"] = yName
	plot1d.plot1d_mpl(plotdict)
	utils.setaxislimits(plotdict)
	labels.add_labels(plotdict)
	utils.save(plotdict)

def invCutEffPlot(plotdict):
	""" Plot inverted cut efficiencies for 1D histograms. """
	cutEffPlot(plotdict, invertCut=True, yName="1 - Efficiency")


def bkgRejVsSigEffPlot(plotdict, invertCutX=False, invertCutY=True,
                       xName="Signal Efficiency", yName="Background Rejection"):
	""" Plot background rejection vs. signal efficiency for 1D histograms. """
	
	plot1d.get_root_histos(plotdict)
	
	# determine indices for multiple plots
	plots = {}
	for index, group in enumerate(plotdict["groups"]):
		plotLabel = group.lower().replace("sig", "").replace("bkg", "")
		if "sig" in group.lower():
			plots.setdefault(plotLabel, {})["sig"] = index
		elif "bkg" in group.lower():
			plots.setdefault(plotLabel, {})["bkg"] = index
	
	# create efficiency graph
	for plotLabel, plotIndices in plots.items():
		sigCutEfficiency = CutEfficiency(plotdict["roothistos"][plotIndices["sig"]])
		bkgCutEfficiency = CutEfficiency(plotdict["roothistos"][plotIndices["bkg"]])
		
		efficiencyGraph = ROOT.TGraph(plotdict["roothistos"][plotIndices["sig"]].GetNbinsX()+1,
		                              array.array("d", sigCutEfficiency.get_cut_efficiencies(invertCut=invertCutX)),
		                              array.array("d", bkgCutEfficiency.get_cut_efficiencies(invertCut=invertCutY)))
		
		plotdict["roothistos"][plotIndices["sig"]] = efficiencyGraph
	
	# modify plotdict
	utils.removeplots(plotdict, [plots[plotLabel]["bkg"] for plotLabel in plots])
	
	plotdict["xname"] = xName
	plotdict["yname"] = yName
	plotdict["xlims"] = [0.0, 1.0] # TODO
	plot1d.plot1d_mpl(plotdict)
	utils.setaxislimits(plotdict)
	labels.add_labels(plotdict)
	utils.save(plotdict)

def bkgRejVsSigRejPlot(plotdict, invertCutX=True, invertCutY=True,
                       xName="Signal Rejection", yName="Background Rejection"):
	""" Plot background rejection vs. signal rejection for 1D histograms. """
	bkgRejVsSigEffPlot(plotdict, invertCutX=invertCutX, invertCutY=invertCutY,
                       xName=xName, yName=yName)

def bkgEffVsSigRejPlot(plotdict, invertCutX=True, invertCutY=False,
                       xName="Signal Rejection", yName="Background Efficiency"):
	""" Plot background efficiency vs. signal rejection for 1D histograms. """
	bkgRejVsSigEffPlot(plotdict, invertCutX=invertCutX, invertCutY=invertCutY,
                       xName=xName, yName=yName)

def bkgEffVsSigEffPlot(plotdict, invertCutX=False, invertCutY=False,
                       xName="Signal Efficiency", yName="Background Efficiency"):
	""" Plot background efficiency vs. signal efficiency for 1D histograms. """
	bkgRejVsSigEffPlot(plotdict, invertCutX=invertCutX, invertCutY=invertCutY,
                       xName=xName, yName=yName)

