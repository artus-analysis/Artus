# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.Utility.progressiterator as pi
import Artus.Utility.jsonTools as jsonTools


class EventSelectionOverlap(analysisbase.AnalysisBase):
	"""EventSelectionOverlap: plot events only in input 1 / overlap / events only in input 2"""
	def __init__(self):
		super(EventSelectionOverlap, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(EventSelectionOverlap, self).modify_argument_parser(parser, args)
		
		parser.set_defaults(x_expressions="1")
		
		self.cutflow_options = parser.add_argument_group("Event selection overlap options")
		self.cutflow_options.add_argument("--events-only-in-1", action="store_true", default=False,
				help="Print events that are only in the first input.")
		self.cutflow_options.add_argument("--events-only-in-2", action="store_true", default=False,
				help="Print events that are only in the second input.")
	
	def prepare_args(self, parser, plotData):
		super(EventSelectionOverlap, self).prepare_args(parser, plotData)
		
		if plotData.plotdict["x_label"] == parser.get_default("x_label"):
			plotData.plotdict["x_label"] = "Event Selection Overlap"
		
		# TODO
		if plotData.plotdict.get("x_tick_labels") == parser.get_default("x_tick_labels"):
			labels_or_nicks = (plotData.plotdict["labels"] if plotData.plotdict["labels"] != None else plotData.plotdict["nicks"])
			labels = filter(lambda label: label != None, labels_or_nicks)
			plotData.plotdict["x_tick_labels"] = [
				"Only " + labels[0],
				"Intersection",
				"Only " + labels[1],
			]
		if plotData.plotdict.get("x_ticks") == parser.get_default("x_ticks"):
			plotData.plotdict["x_ticks"] = [-1, 0, 1]
	
	def run(self, plotData=None):
		super(EventSelectionOverlap, self).run(plotData)
		
		plotData.plotdict["root_objects"] = {}
		for index1, (nick1, tree1) in enumerate(plotData.plotdict.get("root_trees", {}).items()):
			for index2, (nick2, tree2) in enumerate(plotData.plotdict.get("root_trees", {}).items()):
				if index1 != index2 and len(plotData.plotdict["root_objects"]) == 0:

					# Events in first ntuple
					events1 = EventSelectionOverlap.get_events_set_from_tree(tree1, ["run", "lumi", "event"], plotData.plotdict['weights'][0])
					# Events in second ntuple
					events2 = EventSelectionOverlap.get_events_set_from_tree(tree2, ["run", "lumi", "event"], plotData.plotdict['weights'][1])

					events_only1 = list(events2.difference(events1))
					events_intersection = list(events1.intersection(events2))
					events_only2 = list(events1.difference(events2))
					
					print_dict = jsonTools.JsonDict()
					if plotData.plotdict["events_only_in_1"]:
						print_dict.setdefault("RunWhitelist", []).extend(zip(*events_only1)[0])
						print_dict.setdefault("LumiWhitelist", []).extend(zip(*events_only1)[1])
						print_dict.setdefault("EventWhitelist", []).extend(zip(*events_only1)[2])
					if plotData.plotdict["events_only_in_2"]:
						print_dict.setdefault("RunWhitelist", []).extend(zip(*events_only2)[0])
						print_dict.setdefault("LumiWhitelist", []).extend(zip(*events_only2)[1])
						print_dict.setdefault("EventWhitelist", []).extend(zip(*events_only2)[2])
					if len(print_dict) > 0:
						print_dict["MatchRunLumiEventTuples"] = True
						log.info(str(print_dict))

					histogram = ROOT.TH1F("histogram_{0}".format(hashlib.md5("_".join([str(events_only1), str(events_intersection), str(events_only2)]))), "Event Selection Overlap", 3, -1.5, 1.5)

					histogram.SetBinContent(1, len(events_only1))
					histogram.SetBinContent(2, len(events_intersection))
					histogram.SetBinContent(3, len(events_only2))

					# KIT vs sync ntuple
					new_nick = nick2 + "_vs_" + nick1
					plotData.plotdict["root_objects"][new_nick] = histogram
					plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(nick1), new_nick)
					plotData.plotdict["nicks"].remove(nick1)
					plotData.plotdict["nicks"].remove(nick2)
					
	
	@staticmethod
	def get_events_set_from_tree(tree, branch_names, selection):
		return set(EventSelectionOverlap.get_events_list_from_tree(tree, branch_names, selection))

	@staticmethod
	def get_events_list_from_tree(tree, branch_names, selection):
		if isinstance(branch_names, basestring):
			branch_names = [branch_names]
		events = []

		# get list of entry numbers which pass selection
		entrylist_name = tree.GetName() + "entrylist"
		tree.Draw(">>"+entrylist_name, selection)
		root_entrylist = ROOT.gDirectory.Get(entrylist_name)
		entrylist = [root_entrylist.GetEntry(i) for i in range(root_entrylist.GetN())]

		for entry in pi.ProgressIterator(entrylist, description="Reading event numbers from tree"):
			tree.GetEntry(entry)
			event = []
			for branch_name in branch_names:
				event.append(getattr(tree, branch_name))
			events.append(tuple(event))
		return events
					
