# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase
import HarryPlotter.Utility.progressiterator as pi


class EventSelectionOverlap(analysisbase.AnalysisBase):
	def __init__(self):
		super(EventSelectionOverlap, self).__init__()
	
	def run(self, plotData=None):
		super(EventSelectionOverlap, self).run(plotData)
		
		plotData.plotdict["root_objects"] = {}
		for index1, (nick1, tree1) in enumerate(plotData.plotdict.get("root_trees", {}).items()):
			for index2, (nick2, tree2) in enumerate(plotData.plotdict.get("root_trees", {}).items()):
				if index1 != index2 and len(plotData.plotdict["root_objects"]) == 0:

					# Events in sync ntuple
					events1 = EventSelectionOverlap.get_events_set_from_tree(tree1, plotData.plotdict["x_expressions"][index1])
					# Events in KIT ntuple
					events2 = EventSelectionOverlap.get_events_set_from_tree(tree2, plotData.plotdict["x_expressions"][index2])

					n_events_only1 = len(events1.difference(events2))
					n_events_intersection = len(events1.intersection(events2))
					n_events_only2 = len(events2.difference(events1))

					histogram = ROOT.TH1F("histogram_{0}".format(hashlib.md5("_".join([str(n_events_only1), str(n_events_intersection), str(n_events_only2)]))), "Event Selection Overlap", 3, -1.0, 1.0)

					histogram.SetBinContent(1, n_events_only2)
					histogram.SetBinContent(2, n_events_intersection)
					histogram.SetBinContent(3, n_events_only1)

					# KIT vs sync ntuple
					plotData.plotdict["root_objects"][nick2 + "_vs_" + nick1] = histogram
	
	@staticmethod
	def get_events_set_from_tree(tree, events_branch_name):
		events = []
		for entry in pi.ProgressIterator(xrange(tree.GetEntries()), description="Reading event numbers from tree"):
			tree.GetEntry(entry)
			events.append(getattr(tree, events_branch_name))
		return set(events)
					
