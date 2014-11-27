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
	
	def modify_argument_parser(self, parser, args):
		super(EventSelectionOverlap, self).modify_argument_parser(parser, args)
		
		self.cutflow_options = parser.add_argument_group("Event selection overlap options")
		self.cutflow_options.add_argument("--events-only-in-1", action="store_true", default=False,
				help="Print events that are only in the first input.")
		self.cutflow_options.add_argument("--events-only-in-2", action="store_true", default=False,
				help="Print events that are only in the second input.")
	
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

					events_only1 = list(events2.difference(events1))
					events_intersection = list(events1.intersection(events2))
					events_only2 = list(events1.difference(events2))
					
					if plotData.plotdict["events_only_in_1"]:
						log.info("Events only in the first input:\n\t%s" % str(events_only1))
					if plotData.plotdict["events_only_in_2"]:
						log.info("Events only in the second input:\n\t%s" % str(events_only2))

					histogram = ROOT.TH1F("histogram_{0}".format(hashlib.md5("_".join([str(events_only1), str(events_intersection), str(events_only2)]))), "Event Selection Overlap", 3, -1.0, 1.0)

					histogram.SetBinContent(1, len(events_only1))
					histogram.SetBinContent(2, len(events_intersection))
					histogram.SetBinContent(3, len(events_only2))

					# KIT vs sync ntuple
					plotData.plotdict["root_objects"][nick2 + "_vs_" + nick1] = histogram
	
	@staticmethod
	def get_events_set_from_tree(tree, events_branch_name):
		events = []
		for entry in pi.ProgressIterator(xrange(tree.GetEntries()), description="Reading event numbers from tree"):
			tree.GetEntry(entry)
			events.append(getattr(tree, events_branch_name))
		return set(events)
					
