
import array
import copy
import numpy
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


def cutEfficiencyPlot(plotdict, invertCut=False, yName="Efficiency"):
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

def invertedCutEfficiencyPlot(plotdict):
	cutEfficiencyPlot(plotdict, invertCut=True, yName="1 - Efficiency")

