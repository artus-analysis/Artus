#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import copy
import os
import re

import Artus.HarryPlotter.utility.roottools as roottools
import Artus.Utility.tfilecontextmanager as tfilecontextmanager
import Artus.HarryPlotter.harry as harry


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Rename HP-plotable objects in ROOT files", parents=[logger.loggingParser])

	parser.add_argument("root_files", nargs="+", help="Input ROOT file.")
	parser.add_argument("--in-place", default=False, action="store_true",
	                    help="Add renamed objects to the input file. [Default: %(default)s]")
	parser.add_argument("--search-regexs", nargs="+",
	                    help="Regular expressions to seach for in the paths of the ROOT file content.")
	parser.add_argument("--replacements", nargs="+",
	                    help="Replacement strings.")
	parser.add_argument("-a", "--args", default="",
	                    help="Additional Arguments for HarryPlotter. [Default: %(default)s]")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	parser.add_argument("-f", "--n-plots", type=int,
	                    help="Number of plots. [Default: all]")
	

	args = parser.parse_args()
	logger.initLogger(args)
	
	plot_configs = []
	for root_filename in args.root_files:
		config = {
			"files" : [root_filename],
			"output_dir" : os.path.dirname(root_filename),
			"filename" : os.path.basename(root_filename).replace(".root", "" if args.in_place else "_renamed"),
			"plot_modules" : ["ExportRoot"],
		}
		with tfilecontextmanager.TFileContextManager(root_filename, "READ") as root_file:
			elements = roottools.RootTools.walk_root_directory(root_file)
			for index, (key, path) in enumerate(elements):
				class_name = key.GetClassName()
				if any([class_name.startswith(allowed_type) for allowed_type in ["TH", "TProfile", "TGraph", "TF"]]):
					
					label = copy.deepcopy(path)
					for search_regex, replacement in zip(args.search_regexs, args.replacements):
						label = re.sub(search_regex, replacement, label)
					
					if (not args.in_place) or (label != path):
						config.setdefault("x_expressions", []).append(path)
						config.setdefault("labels", []).append(label)
					
					if log.isEnabledFor(logging.DEBUG):
						if label != path:
							log.debug(path + " --> " + label)
		
		plot_configs.append(config)
	
	#if log.isEnabledFor(logging.DEBUG):
	#	import pprint
	#	pprint.pprint(plot_configs)
	harry.HarryPlotter(list_of_config_dicts=plot_configs, list_of_args_strings=args.args, n_processes=args.n_processes, n_plots=args.n_plots)

