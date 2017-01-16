# -*- coding: utf-8 -*

"""
	Just opens a ROOT file and stores the ROOT/RooFit content in the root_objects.
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.inputbase as inputbase


class InputRootSimple(inputbase.InputBase):

	def modify_argument_parser(self, parser, args):
		super(InputRootSimple, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("-i", "--files", type=str, nargs="+",
		                                default='', help="Input (root) file(s).")
		self.input_options.add_argument("--root-names", nargs="+",
		                                default=[], help="Name(s) of ROOT/RooFit objects to retrieve.")
		self.input_options.add_argument("--hide-progressbar", nargs ="?", type="bool", default=False, const=True,
		                                help="Show progress of the individual plot. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(InputRootSimple, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["root_names"])

		if plotData.plotdict.get("nicks", None) == [None]:
			plotData.plotdict["nicks"] = []

		if all([item == [None] for item in [plotData.plotdict.get(item2+"_expressions", None) for item2 in ['x', 'y', 'z']]]) and plotData.plotdict["filename"] == None:
			plotData.plotdict["filename"] = None

	def run(self, plotData):
		for index, filename in enumerate(plotData.plotdict["files"]):
			file = ROOT.TFile(filename)

			for name in plotData.plotdict["root_names"]:
				root_obj = file.Get(name)
				plotData.plotdict.setdefault("nicks", []).append(name)
				plotData.plotdict.setdefault("root_objects", {})[name] = root_obj
