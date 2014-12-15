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

import Artus.HarryPlotter.input_modules.inputfile as inputfile
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.extrafunctions as extrafunctions
import Artus.Utility.progressiterator as pi


class InputRoot(inputfile.InputFile):
	def __init__(self):
		super(InputRoot, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(InputRoot, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("--folders", type=str, nargs='*',
		                                help="Path(s) to ROOT objects.")
		self.input_options.add_argument("--friend-treenames", type=str, nargs="+",
		                                help="Names of trees to be used as friends. Seperate different plots with space, seperate for same plot with space.", default=None)
		self.input_options.add_argument("--friend-filenames", type=str, nargs="+",
		                                help="Filenames to be added as friends. Seperate different plots with space, seperate for same plot with space.", default=None)
		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-axis variable expression(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-axis variable expression(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-axis variable expression(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+", default="1.0",
		                                help="Weight (cut) expression(s). [Default: %(default)s]")
		
		self.input_options.add_argument("--x-bins", type=str, nargs='+', default=["25"],
		                                help="Bining for x-axis. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [Default: %(default)s]")
		self.input_options.add_argument("--y-bins", type=str, nargs='+', default=["25"],
		                                help="Bining for y-axis of 2D/3D histograms. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [Default: %(default)s]")
		self.input_options.add_argument("--z-bins", type=str, nargs='+', default=["25"],
		                                help="Bining for z-axis of 3D histograms. In case only one argument is specified, is is taken as for the first parameter of TTree::Draw. Multiple arguments specify custom bin edgeds. [Default: %(default)s]")
		self.input_options.add_argument("--root-histogram-draw-options", type=str, default="",
		                                help="Optional argument for TTree:Draw() call. Use e.g. 'prof' or 'profs' for projections of 2D-Histograms to 1D. See also http://root.cern.ch/root/html/TTree.html#TTree:Draw.")

	def prepare_args(self, parser, plotData):
		super(InputRoot, self).prepare_args(parser, plotData)

		if (plotData.plotdict["friend_treenames"] is not None) and (plotData.plotdict["friend_filenames"] is not None):
			plotData.plotdict["friend_trees"] = []
			for friend_treename, friend_filename in zip( 
		                     	 plotData.plotdict["friend_treenames"],
		                     	 plotData.plotdict["friend_filenames"]):
				tuple = []
				for plot_friend_treename, plot_friend_filename in zip(friend_treename.split(" "), friend_filename.split(" ")):
					tuple.append((plot_friend_treename, plot_friend_filename))
				plotData.plotdict["friend_trees"].append(tuple)
		else:
			plotData.plotdict["friend_trees"] = [None]

		self.prepare_list_args(plotData, ["files", "folders", "weights", "friend_trees"])
		plotData.plotdict["folders"] = [folders.split() if folders else [""] for folders in plotData.plotdict["folders"]]
	def run(self, plotData):
		
		if plotData.plotdict["quantities"]:
			extrafunctions.print_quantities(root_files=plotData.plotdict["files"], root_folders=plotData.plotdict["folders"])
			sys.exit(0)
		root_tools = roottools.RootTools()
		for index, (root_files, folders, x_expression, y_expression, z_expression, weight, nick, friend_trees) in enumerate(pi.ProgressIterator(zip(*
			[plotData.plotdict[key] for key in ["files", "folders", "x_expressions", "y_expressions", "z_expressions", "weights", "nicks", "friend_trees"]]),
			description="Reading ROOT inputs")):
			# check whether to read from tree or directly from histograms
			root_object_type = roottools.RootTools.check_type(root_files, folders)
			root_tree_chain = None
			root_histogram = None
			
			if root_object_type == ROOT.TTree:
				variable_expression = "%s%s%s" % (z_expression + ":" if z_expression else "",
				                                  y_expression + ":" if y_expression else "",
				                                  x_expression)
				option = plotData.plotdict["root_histogram_draw_options"]	
				root_tree_chain, root_histogram = root_tools.histogram_from_tree(
						root_files, folders,
						x_expression, y_expression, z_expression,
						x_bins=plotData.plotdict["x_bins"],
						y_bins=plotData.plotdict["y_bins"],
						z_bins=plotData.plotdict["z_bins"],
						weight_selection=weight, option=option, name=None,
						friend_trees=friend_trees
				)
				
			elif root_object_type == ROOT.TH1:
				if x_expression is None:
					log.error('No x_expression provided.')
					sys.exit(1)
				root_objects = [os.path.join(folder, x_expression) for folder in folders]
				
				root_histogram = roottools.RootTools.histogram_from_file(root_files, root_objects, name=None)
			
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
