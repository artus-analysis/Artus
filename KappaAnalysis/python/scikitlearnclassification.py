# -*- coding: utf-8 -*-

import numpy as np
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import GradientBoostingClassifier  

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


def tmva_classification(args_from_script=None):
	"""
	Perform scikit learn classification training.
	
	"""

	parser = argparse.ArgumentParser(description="Perform scikit learn classification training.",
	                                 fromfile_prefix_chars="@", conflict_handler="resolve",
	                                 parents=[logger.loggingParser])

	parser.add_argument("-o", "--output-file", default="sklearnClassification/output.root",
	                    help="Output file. [Default: %(default)s]")
	parser.add_argument("-n", "--name", default="training",
	                    help="Training name. [Default: %(default)s]")
	parser.add_argument("-cv", "--cross-validation", default=False,
	                    help="Cross Validation. [Default: %(default)s]")
	parser.add_argument("-v", "--variables", nargs="+", required=True, default=None,
	                    help="Training variables. Multiple arguments for TMVA.Factory.AddVariable are split by semicolon.")
	parser.add_argument("-s", "--signal-trees", nargs="+", required=True, default=None,
	                    help="Signal trees. Format same as for TChain.Add: path/to/file.root/path/to/tree.")
	parser.add_argument("--signal-tree-weights", nargs="+", type=float, default=[1.0],
	                    help="Weights for signal trees. Need same number of arguments as for --signal-trees or one.")
	parser.add_argument("--signal-event-weight", help="Signal event weight expression.")
	parser.add_argument("--signal-cuts", default="", help="Signal cut expression. [Default: %(default)s]")
	parser.add_argument("--k-fold", default="2", help="K fold iteration. [Default: %(default)s]")
	parser.add_argument("--splitting", default="0.5", help="Splitting training and testing. Give value for relative size of test sample. [Default: %(default)s]")
	parser.add_argument("-b", "--background-trees", nargs="+", required=True, default=None,
	                    help="Background trees. Format same as for TChain.Add: path/to/file.root/path/to/tree.")
	parser.add_argument("--background-tree-weights", nargs="+", type=float, default=[1.0],
	                    help="Weights for background trees. Need same number of arguments as for --background-trees or one.")
	parser.add_argument("--background-event-weight", help="Background event weight expression.")
	parser.add_argument("--background-cuts", default="", help="Background cut expression. [Default: %(default)s]")	
	parser.add_argument("-m", "--methods", nargs="+", required=True, default=None,
	                    help="MVA methods. Multiple arguments for TMVA.Factory.BookMethod are split by semicolon. Format: name;title;options")

	args = parser.parse_args(args_from_script.split() if args_from_script != None else None)
	logger.initLogger(args)

	
	#training variables	
	list_of_variables = [variable for variable in args.variables.split(";")]

	#preparing signal trees
	signal = root2array(args.signal_trees, "tree", list_of_variables)
	signal = rec2array(signal)
	
	#preparing background trees
	background = root2array(args.background_trees, "tree", list_of_variables)
	background = rec2array(background)

	X = np.concatenate((signal,background))
	Y = np.concatenate((np.ones(signal.shape[0]), np.zeros(background.shape[0])))	
	
	#cross-validation
	if cross_validation:
		X_train, X_test, y_train, y_test = train_test_split(X, Y, test_size = args.splitting, random_state = 0)
		cv = StratifiedShuffleSplit(X_train.shape[0], n_iter = args.k_fold, test_size = args.splitting, random_state = 0)

	#boosting and training
	gbt = GradientBoostingClassifier(args.methods).fit(X_train, y_train)
	scores = cross_validation.cross_val_score(gbt, X_test, y_test, n_jobs=-1, cv=cv)




















			
	

