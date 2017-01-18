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
import Artus.Utility.jsonTools as jsonTools
from Artus.Utility.tfilecontextmanager import TFileContextManager


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
		self.input_options.add_argument("-q", "--quantities", nargs="?", type="bool", default=False, const=True,
		                                help="Print available quantities in given folder. [Default: %(default)s]")
		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-axis variable expression(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-axis variable expression(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-axis variable expression(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+", default="1.0",
		                                help="Weight (cut) expression(s). See https://root.cern.ch/root/html/TTree.html#TTree:Draw@2 under 'selection...' for an explanation. [Default: %(default)s]")
		
		self.input_options.add_argument("--x-bins", type=str, nargs='+', default=[None],
		                                help="Binnings for x-axis. Possible inputs:\
		                                      --x-bins \"25\" for nBins; --x-bins \"20,0,1000\" for nBins, lower and upper limit;\
		                                      --x-bins \"0 10 20 30 50 100 100\" for custom bin widths.\
		                                      [Default: [\"25\"] for trees, no rebinning for histograms]")
		self.input_options.add_argument("--y-bins", type=str, nargs='+', default=[None],
		                                help="Binnings for y-axis of 2D/3D histograms. See help for --x-bins for more information. [Default: [\"25\"] for trees, no rebinning for histograms]")
		self.input_options.add_argument("--z-bins", type=str, nargs='+', default=[None],
		                                help="Binnings for z-axis of 3D histograms. See help for --x-bins for more information. [Default: [\"25\"] for trees, no rebinning for histograms]")
		
		self.input_options.add_argument("--tree-draw-options", nargs='+', type=str, default="",
		                                help="Optional argument for TTree:Draw() call. Use e.g. \"prof\" or \"profs\" for projections of 2D-Histograms to 1D. See also http://root.cern.ch/ooot/html/TTree.html#TTree:Draw. Specify \"TGraph\" for plotting y- vs. x-values into a TGraph. \"TGraphErrors\" leads to a graph with errors by specifying inputs with --x-expressions <x values>:<x errors> --y-expressions <y values>:<y errors>. \"TGraphAsymmErrorsX\" leads to a graph with asymmetric x-errors by specifying inputs with --x-expressions <x values>:<x errors (down)>:<x errors (up)> --y-expressions <y values>. \"TGraphAsymmErrorsY\" leads to a graph with asymmetric y-errors by specifying inputs with --x-expressions <x values> --y-expressions <y values>:<y errors (down)>:<y errors (up)>. ROOT.TTree.MakeProxy and ROOT.TTree.Process is usued to fill the histograms from a tree instead of ROOT.TTree.Draw or ROOT.TTree.Project in case you specify the option \"proxy\" This is needed e.g. for formulas containing two branches/leafs with different non-fundamental types.")
		
		self.input_options.add_argument("--keep-trees", nargs="?", type="bool", default=False, const=True,
		                                help="Keep trees in the plot data object during the complete run. [Default: %(default)s]")
		self.input_options.add_argument("--read-config", nargs="?", type="bool", default=False, const=True,
		                                help="Read in the config stored in Artus ROOT outputs. [Default: %(default)s]")
		self.input_options.add_argument("--hide-progressbar", nargs ="?", type="bool", default=False, const=True,
		                                help="Show progress of the individual plot. [Default: %(default)s]")

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

		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "x_bins", "y_bins", "z_bins", "scale_factors", "files", "directories", "folders", "weights", "friend_trees", "tree_draw_options"], help="InputRoot options")
		inputbase.InputBase.prepare_nicks(plotData)
		
		plotData.plotdict["folders"] = [folders.split() if folders else [""] for folders in plotData.plotdict["folders"]]
		
		if plotData.plotdict["read_config"]:
			self.read_input_json_dicts(plotData)

	def run(self, plotData):
		
		root_tools = roottools.RootTools()
		self.hide_progressbar = plotData.plotdict["hide_progressbar"]
		del(plotData.plotdict["hide_progressbar"])
		for index, (
				root_files,
				folders,
				x_expression,
				y_expression,
				z_expression,
				weight,
				x_bins,
				y_bins,
				z_bins,
				nick,
				friend_trees,
				option
		) in enumerate(pi.ProgressIterator(zip(
				plotData.plotdict["files"],
				plotData.plotdict["folders"],
				[self.expressions.replace_expressions(expression) for expression in plotData.plotdict["x_expressions"]],
				[self.expressions.replace_expressions(expression) for expression in plotData.plotdict["y_expressions"]],
				[self.expressions.replace_expressions(expression) for expression in plotData.plotdict["z_expressions"]],
				[self.expressions.replace_expressions(expression) for expression in plotData.plotdict["weights"]],
				plotData.plotdict["x_bins"],
				plotData.plotdict["y_bins"],
				plotData.plotdict["z_bins"],
				plotData.plotdict["nicks"],
				plotData.plotdict["friend_trees"],
				plotData.plotdict["tree_draw_options"]
		), description="Reading ROOT inputs", visible=not self.hide_progressbar )):
			# check whether to read from TTree or from TDirectory
			root_folder_type = roottools.RootTools.check_type(root_files, folders,
			                                                  print_quantities=plotData.plotdict["quantities"])
			root_tree_chain = None
			root_histogram = None
			
			if root_folder_type == "TTree":
				variable_expression = "%s%s%s" % (z_expression + ":" if z_expression else "",
				                                  y_expression + ":" if y_expression else "",
				                                  x_expression)
				root_tree_chain, root_histogram = root_tools.histogram_from_tree(
						root_files, folders,
						x_expression, y_expression, z_expression,
						x_bins=["25"] if x_bins is None else x_bins,
						y_bins=["25"] if y_bins is None else y_bins,
						z_bins=["25"] if z_bins is None else z_bins,
						weight_selection=weight, option=option, name=None,
						friend_trees=friend_trees
				)
				
			elif root_folder_type == "TDirectory":
				if x_expression is None:
					log.error('No x_expression provided.')
					sys.exit(1)
				root_objects = [os.path.join(folder, x_expression) for folder in folders]
				
				root_histogram = roottools.RootTools.histogram_from_file(
						root_files,
						root_objects,
						x_bins=x_bins,
						y_bins=y_bins,
						z_bins=z_bins,
						name=None)
				if hasattr(root_histogram, "Sumw2"):
					root_histogram.Sumw2()
			else:
				log.critical("Error getting ROOT object from file. Exiting.")
				sys.exit(1)
			
			log.debug("Input object %d (nick %s):" % (index, nick))
			if log.isEnabledFor(logging.DEBUG):
				root_histogram.Print()
			
			# save tree (chain) in plotData merging chains with same nick names
			if (not root_tree_chain is None) and plotData.plotdict["keep_trees"]:
				if nick in plotData.plotdict.setdefault("root_trees", {}):
					plotData.plotdict["root_trees"][nick].Add(root_tree_chain)
				else:
					plotData.plotdict["root_trees"][nick] = root_tree_chain
			
			# save histogram in plotData
			# merging histograms with same nick names is done in upper class
			plotData.plotdict.setdefault("root_objects", {}).setdefault(nick, []).append(root_histogram)

		# run upper class function at last
		
		super(InputRoot, self).run(plotData)


	def read_input_json_dicts(self, plotData):
		"""If Artus config dict is present in root file -> append to plotdict"""
		for root_files in plotData.plotdict["files"]:
			# TODO: make TChain instead of using only first file?
			with TFileContextManager(root_files[0], "READ") as tfile:
				keys, names = zip(*roottools.RootTools.walk_root_directory(tfile))
			if jsonTools.JsonDict.PATH_TO_ROOT_CONFIG in names:
				input_json_dict = jsonTools.JsonDict(root_files)
			else:
				input_json_dict = {}
			plotData.input_json_dicts.append(input_json_dict)

		# Raise warning if config dict could be read out for some, but not for all files
		if ({} in plotData.input_json_dicts and not all([i == {} for i in plotData.input_json_dicts])):
			log.warning("'config' dict could not be read for all input files! (ignore this warning if you're not using Artus output files)")
