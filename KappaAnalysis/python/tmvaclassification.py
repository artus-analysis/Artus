# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


def tmva_classification(args_from_script=None):
	"""
	Perform TMVA classification training.
	
	http://root.cern.ch/svn/root/trunk/tmva/test/TMVAClassification.C
	http://root.cern.ch/root/htmldoc/TMVA__Factory.html
	http://root.cern.ch/root/htmldoc/TMVA_Index.html
	"""

	parser = argparse.ArgumentParser(description="Perform TMVA classification training.",
	                                 fromfile_prefix_chars="@", conflict_handler="resolve",
	                                 parents=[logger.loggingParser])

	parser.add_argument("-o", "--output-file", default="tmvaClassification/output.root",
	                    help="Output file. [Default: %(default)s]")
	parser.add_argument("-n", "--name", default="training",
	                    help="Training name. [Default: %(default)s]")
	parser.add_argument("--factory-options", default="",
	                    help="Options for TMVA.Factory constructor. [Default: %(default)s]")
	parser.add_argument("-v", "--variables", nargs="+", required=True, default=None,
	                    help="Training variables. Multiple arguments for TMVA.Factory.AddVariable are split by semicolon.")
	parser.add_argument("--spectator-variables", nargs="+", default=[],
	                    help="Spectator variables. Multiple arguments for TMVA.Factory.AddSpectator are split by semicolon. [Default: %(default)s]")
	
	parser.add_argument("-s", "--signal-trees", nargs="+", required=True, default=None,
	                    help="Signal trees. Format same as for TChain.Add: path/to/file.root/path/to/tree.")
	parser.add_argument("--signal-tree-weights", nargs="+", type=float, default=[1.0],
	                    help="Weights for signal trees. Need same number of arguments as for --signal-trees or one.")
	parser.add_argument("--signal-event-weight", help="Signal event weight expression.")
	parser.add_argument("--signal-cuts", default="", help="Signal cut expression. [Default: %(default)s]")
	
	parser.add_argument("-b", "--background-trees", nargs="+", required=True, default=None,
	                    help="Background trees. Format same as for TChain.Add: path/to/file.root/path/to/tree.")
	parser.add_argument("--background-tree-weights", nargs="+", type=float, default=[1.0],
	                    help="Weights for background trees. Need same number of arguments as for --background-trees or one.")
	parser.add_argument("--background-event-weight", help="Background event weight expression.")
	parser.add_argument("--background-cuts", default="", help="Background cut expression. [Default: %(default)s]")
	
	parser.add_argument("--preparation-trees-options", default="",
	                    help="Options for preparation of inputs trees as passed to TMVA.Factory.PrepareTrainingAndTestTree. [Default: %(default)s]")
	
	parser.add_argument("-m", "--methods", nargs="+", required=True, default=None,
	                    help="MVA methods. Multiple arguments for TMVA.Factory.BookMethod are split by semicolon. Format: name;title;options")

	args = parser.parse_args(args_from_script.split() if args_from_script != None else None)
	logger.initLogger(args)
	
	# load the library
	ROOT.TMVA.Tools.Instance()
	
	# create output file
	if not os.path.exists(os.path.dirname(args.output_file)):
		os.makedirs(os.path.dirname(args.output_file))
	output_file = ROOT.TFile(args.output_file, "RECREATE")
	
	# create factory
	log.debug("TMVA.Factory(\"" + args.name + "\", TFile(\"" + args.output_file + "\", \"RECREATE\"), \"" + args.factory_options + "\")")
	tmva_factory = ROOT.TMVA.Factory(args.name, output_file, args.factory_options)
	
	# add training variables
	for variable in args.variables:
		log.debug("TMVA.Factory.AddVariable(" + ", ".join(["\"" + v + "\"" for v in variable.split(";")]) + ")")
		tmva_factory.AddVariable(*(variable.split(";")))
	
	# add spectator variables
	for spectator_variable in args.spectator_variables:
		log.debug("TMVA.Factory.AddSpectator(" + ", ".join(["\"" + v + "\"" for v in spectator_variable.split(";")]) + ")")
		tmva_factory.AddSpectator(*(spectator_variable.split(";")))
	
	# add signal
	if len(args.signal_tree_weights) != 1 and len(args.signal_tree_weights) != len(args.signal_trees):
		log.warning("Number of signal tree weights need to be 1 or " + len(args.signal_trees) + "!")
	args.signal_tree_weights = (args.signal_tree_weights * len(args.signal_trees))[:len(args.signal_trees)]
	
	for signal_tree_parameter, signal_tree_weight in zip(args.signal_trees, args.signal_tree_weights):
		log.debug("TMVA.Factory.AddSignalTree(TChain.Add(\"" + signal_tree_parameter + "\"), " + str(signal_tree_weight) + ")")
		signal_tree = ROOT.TChain()
		signal_tree.Add(signal_tree_parameter)
		tmva_factory.AddSignalTree(signal_tree, signal_tree_weight)
	
	if args.signal_event_weight:
		log.debug("TMVA.Factory.SetSignalWeightExpression(\"" + args.signal_event_weight + "\")")
		tmva_factory.SetSignalWeightExpression(args.signal_event_weight)
	
	# add background
	if len(args.background_tree_weights) != 1 and len(args.background_tree_weights) != len(args.background_trees):
		log.warning("Number of background tree weights need to be 1 or " + len(args.background_trees) + "!")
	args.background_tree_weights = (args.background_tree_weights * len(args.background_trees))[:len(args.background_trees)]
	
	for background_tree_parameter, background_tree_weight in zip(args.background_trees, args.background_tree_weights):
		log.debug("TMVA.Factory.AddBackgroundTree(TChain.Add(\"" + background_tree_parameter + "\"), " + str(background_tree_weight) + ")")
		background_tree = ROOT.TChain()
		background_tree.Add(background_tree_parameter)
		tmva_factory.AddBackgroundTree(background_tree, background_tree_weight)
	
	if args.background_event_weight:
		log.debug("TMVA.Factory.SetBackgroundWeightExpression(\"" + args.background_event_weight + "\")")
		tmva_factory.SetBackgroundWeightExpression(args.background_event_weight)
	
	# prepare trees
	log.debug("TMVA.Factory.PrepareTrainingAndTestTree(\"" + args.signal_cuts + "\", \"" + args.background_cuts + "\", \"" + args.preparation_trees_options + "\")")
	tmva_factory.PrepareTrainingAndTestTree(ROOT.TCut(args.signal_cuts), ROOT.TCut(args.background_cuts),
	                                        args.preparation_trees_options)
	
	# book methods
	for method in args.methods:
		log.debug("TMVA.Factory.BookMethod(" + ", ".join(["\"" + m + "\"" for m in method.split(";")]) + ")")
		tmva_factory.BookMethod(*(method.split(";")))
	
	# perform full training
	log.debug("TMVA.Factory.TrainAllMethods()")
	tmva_factory.TrainAllMethods()
	
	log.debug("TMVA.Factory.TestAllMethods()")
	tmva_factory.TestAllMethods()
	
	log.debug("TMVA.Factory.EvaluateAllMethods()")
	tmva_factory.EvaluateAllMethods()
	
	# finish
	output_file.Close()
	log.info("Training output is written to \"" + args.output_file + "\".")

