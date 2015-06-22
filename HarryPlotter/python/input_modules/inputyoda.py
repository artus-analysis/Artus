# -*- coding: utf-8 -*-

"""
	Creates ROOT histograms from YODA files.
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import yoda

import Artus.HarryPlotter.inputbase as inputbase


class InputYoda(inputbase.InputBase):

	def modify_argument_parser(self, parser, args):
		self.yoda_options = parser.add_argument_group("YODA input options")
		self.yoda_options.add_argument("--yoda-files", type=str, nargs="+",
		                                help="Input (yoda) file(s).")

	def prepare_args(self, parser, plotData):
		# set default for output filename
		if plotData.plotdict["nicks"] is None:
			plotData.plotdict["nicks"] = []
		if all([item == [None] for item in [plotData.plotdict.get(item2+"_expressions", None) for item2 in ['x', 'y', 'z']]]) and plotData.plotdict["filename"] == None:
			plotData.plotdict["filename"] = "yoda"

	def run(self, plotData):
		"""Iterate over files,convert to ROOT and append."""
		for index, filename in enumerate(plotData.plotdict['yoda_files']):
			yoda_objects = yoda.readYODA(filename)
			root_objects = [yoda.to_root(obj) for obj in yoda_objects.values()]
			for obj in root_objects:
				name = obj.GetName().replace("/", "")
				obj.SetName(name)
				plotData.plotdict.setdefault("nicks", []).append(name)
				plotData.plotdict.setdefault("root_objects", {})[name] = obj
