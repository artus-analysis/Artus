# -*- coding: utf-8 -*-

"""
Copy/replace/rename nicks of root objects. 
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class Rename(analysisbase.AnalysisBase):
	def __init__(self):
		super(Rename, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(Rename, self).modify_argument_parser(parser, args)
		
		self.rename_options = parser.add_argument_group("Rename options")
		self.rename_options.add_argument("--rename-src-nicks", nargs="+",
				help="Nick names to be renamed/moved.")
		self.rename_options.add_argument("--rename-dst-nicks", nargs="+",
				help="Final nick names for the moved objects.")

	def prepare_args(self, parser, plotData):
		super(Rename, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["rename_src_nicks", "rename_dst_nicks"])
		
		for src_nick, dst_nick in zip(plotData.plotdict["rename_src_nicks"], plotData.plotdict["rename_dst_nicks"]):
			if not dst_nick in plotData.plotdict["nicks"]:
				if src_nick in plotData.plotdict["nicks"] and src_nick in plotData.plotdict["root_objects"].keys():
					plotData.plotdict["nicks"].insert(plotData.plotdict["nicks"].index(src_nick), dst_nick)
				else:
					log.warning("Input {src} not found and therefore cannot be renamed/moved!".format(src=src_nick))
	
	def run(self, plotData=None):
		super(Rename, self).run(plotData)
		
		for src_nick, dst_nick in zip(plotData.plotdict["rename_src_nicks"], plotData.plotdict["rename_dst_nicks"]):
			if src_nick in plotData.plotdict["nicks"] and src_nick in plotData.plotdict["root_objects"].keys():
				if dst_nick in plotData.plotdict["root_objects"].keys():
					log.debug("Replaced nick \"{dst}\" by nick \"{src}\".".format(src=src_nick, dst=dst_nick))
				else:
					log.debug("Copied nick \"{src}\" to nick \"{dst}\".".format(src=src_nick, dst=dst_nick))
				plotData.plotdict["root_objects"][dst_nick] = plotData.plotdict["root_objects"][src_nick]

