# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import os
import ROOT

import Artus.HarryPlotter.inputbase as inputbase
import Artus.HarryPlotter.roottools as roottools

ROOT.PyConfig.IgnoreCommandLineOptions = True


class InputRoot(inputbase.InputBase):
	def __init__(self):
		inputbase.InputBase.__init__(self)
	
	def modify_argument_parser(self, parser):
		inputbase.InputBase.modify_argument_parser(self, parser)
		self.input_options.add_argument("--folders", type=str, nargs='*',
		                                help="Path(s) to ROOT objects.")
	
	def prepare_args(self, plotData):
		inputbase.InputBase.prepare_args(self, plotData)
		
		self.prepare_list_args(plotData, ["files", "folders"])
		plotData.plotdict["folders"] = [folders.split() if folders else [""] for folders in plotData.plotdict["folders"]]
	
	def run(self, plotData):
		inputbase.InputBase.run(self, plotData)
		for root_files, folders, x_expression, y_expression, z_expression, weight, nick in zip(*
			[plotData.plotdict[key] for key in ["files", "folders", "x_expressions", "y_expressions", "z_expressions", "weights", "nicks"]]):
			
			# check wether to read from tree or directly from histograms
			root_object_type = roottools.check_type(root_files, folders)
			root_histogram = None
			
			if root_object_type == ROOT.TTree:
				variable_expression = "%s%s%s" % (z_expression + ":" if z_expression else "",
				                                  y_expression + ":" if y_expression else "",
				                                  x_expression)
				
				root_histogram = roottools.histogram_from_tree(root_files, folders, variable_expression,
				                                               binning=None, weight_selection=weight, option="", name=None)
				
			elif root_object_type == ROOT.TH1:
				root_objects = [os.path.join(folder, x_expression) for folder in folders]
				
				root_histogram = roottools.histogram_from_file(root_files, root_objects, name=None)
			
			# save histogram in plotData merging histograms with same nick names
			if nick in plotData.plotdict.setdefault("root_histos", {}):
				plotData.plotdict["root_histos"][nick].Add(root_histogram)
			else:
				plotData.plotdict["root_histos"][nick] = root_histogram

