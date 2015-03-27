# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import os
import sys
import ROOT

import Artus.HarryPlotter.inputbase as inputbase
import Artus.HarryPlotter.input_modules.inputfile as inputfile
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.Utility.progressiterator as pi


class InputRoot(inputfile.InputFile):
	def __init__(self):
		super(InputRoot, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(InputRoot, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("-f", "--folders", type=str, nargs='*',
		                                help="Path(s) to ROOT objects.")
		self.input_options.add_argument("--friend-treenames", type=str, nargs="+",
		                                help="Names of trees to be used as friends. Seperate different plots with space, seperate for same plot with whitespace.", default=None)
		self.input_options.add_argument("--friend-filenames", type=str, nargs="+",
		                                help="Filenames to be added as friends. Seperate different plots with space, seperate for same plot with whitespace.", default=None)
		self.input_options.add_argument("--quantities", action="store_true", default=False,
		                                help="Print available quantities in given folder")
		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-axis variable expression(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-axis variable expression(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-axis variable expression(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+", default="1.0",
		                                help="Weight (cut) expression(s). [Default: %(default)s]")
		
		self.input_options.add_argument("--x-bins", type=str, nargs='+', default=None,
		                                help="Bining for x-axis. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [Default: [\"25\"] for trees, no rebinning for histograms]")
		self.input_options.add_argument("--y-bins", type=str, nargs='+', default=None,
		                                help="Bining for y-axis of 2D/3D histograms. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [\"25\"] for trees, no rebinning for histograms]")
		self.input_options.add_argument("--z-bins", type=str, nargs='+', default=None,
		                                help="Bining for z-axis of 3D histograms. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [\"25\"] for trees, no rebinning for histograms]")
		self.input_options.add_argument("--tree-draw-options", nargs='+', type=str, default="",
		                                help="Optional argument for TTree:Draw() call. Use e.g. 'prof' or 'profs' for projections of 2D-Histograms to 1D. See also http://root.cern.ch/ooot/html/TTree.html#TTree:Draw. Specify \"TGraph\" for plotting y- vs. x-values into a TGraph. \"TGraphErrors\" leads to a graph with errors by specifying inputs with --x-expressions <x values>:<x errors> --y-expressions <y values>:<y errors>. \"TGraphAsymmErrorsX\" leads to a graph with asymmetric x-errors by specifying inputs with --x-expressions <x values>:<x errors (down)>:<x errors (up)> --y-expressions <y values>. \"TGraphAsymmErrorsY\" leads to a graph with asymmetric y-errors by specifying inputs with --x-expressions <x values> --y-expressions <y values>:<y errors (down)>:<y errors (up)>.")

	def prepare_args(self, parser, plotData):
		super(InputRoot, self).prepare_args(parser, plotData)

		if (plotData.plotdict["friend_treenames"] is not None) and (plotData.plotdict["friend_filenames"] is not None):
			plotData.plotdict["friend_trees"] = []
			for friend_treename, friend_filename in zip( 
		                     	 plotData.plotdict["friend_treenames"],
		                     	 plotData.plotdict["friend_filenames"]):
				tuple = []
				for plot_friend_treename, plot_friend_filename in zip(friend_treename.split(), friend_filename.split()):
					tuple.append((plot_friend_treename, plot_friend_filename))
				plotData.plotdict["friend_trees"].append(tuple)
		else:
			plotData.plotdict["friend_trees"] = [None]

		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "scale_factors", "files", "directories", "folders", "weights", "friend_trees", "tree_draw_options"])
		plotData.plotdict["folders"] = [folders.split() if folders else [""] for folders in plotData.plotdict["folders"]]
		
		inputbase.InputBase.prepare_nicks(plotData)
	
	def run(self, plotData):
		
		root_tools = roottools.RootTools()
		for index, (root_files, folders, x_expression, y_expression, z_expression, weight, nick, friend_trees, option) in enumerate(pi.ProgressIterator(zip(*
			[plotData.plotdict[key] for key in ["files", "folders", "x_expressions", "y_expressions", "z_expressions", "weights", "nicks", "friend_trees", "tree_draw_options"]]),
			description="Reading ROOT inputs")):
			# check whether to read from tree or directly from histograms
			root_object_type = roottools.RootTools.check_type(root_files, folders,
			                                                  print_quantities=plotData.plotdict["quantities"])
			root_tree_chain = None
			root_histogram = None
			
			if root_object_type == ROOT.TTree:
				variable_expression = "%s%s%s" % (z_expression + ":" if z_expression else "",
				                                  y_expression + ":" if y_expression else "",
				                                  x_expression)
				weight = self.auto_detect_type_and_modify_weight(weight, root_files, plotData)
				root_tree_chain, root_histogram = root_tools.histogram_from_tree(
						root_files, folders,
						x_expression, y_expression, z_expression,
						x_bins=["25"] if plotData.plotdict["x_bins"] is None else plotData.plotdict["x_bins"],
						y_bins=["25"] if plotData.plotdict["y_bins"] is None else plotData.plotdict["y_bins"],
						z_bins=["25"] if plotData.plotdict["z_bins"] is None else plotData.plotdict["z_bins"],
						weight_selection=weight, option=option, name=None,
						friend_trees=friend_trees
				)
				
			elif root_object_type == ROOT.TH1:
				if x_expression is None:
					log.error('No x_expression provided.')
					sys.exit(1)
				root_objects = [os.path.join(folder, x_expression) for folder in folders]
				
				root_histogram = roottools.RootTools.histogram_from_file(
						root_files,
						root_objects,
						x_bins=plotData.plotdict["x_bins"],
						y_bins=plotData.plotdict["y_bins"],
						z_bins=plotData.plotdict["z_bins"],
						name=None)
			
			log.debug("Input object %d (nick %s):" % (index, nick))
			if log.isEnabledFor(logging.DEBUG):
				root_histogram.Print()
			
			# save tree (chain) in plotData merging chains with same nick names
			if root_tree_chain != None:
				if nick in plotData.plotdict.setdefault("root_trees", {}):
					plotData.plotdict["root_trees"][nick].Add(root_tree_chain)
				else:
					plotData.plotdict["root_trees"][nick] = root_tree_chain
			
			# save histogram in plotData
			# merging histograms with same nick names is done in upper class
			plotData.plotdict.setdefault("root_objects", {}).setdefault(nick, []).append(root_histogram)

		# run upper class function at last
		super(InputRoot, self).run(plotData)

	def auto_detect_type_and_modify_weight(self, weight, root_files, plotData):
		""" This function checks the type (data/MC) of the input file(s) and
			modifies the weights accordingly. Actual implementation is
			analysis-specific.
		"""
		return weight
