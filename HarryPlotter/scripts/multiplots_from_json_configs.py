#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import Artus.Utility.jsonTools as jsonTools
import Artus.HarryPlotter.harry as harry


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate multiple plots from JSON configs.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("-j", "--json-configs", nargs="+",
	                    help="JSON config files.")
	parser.add_argument("-i", "--input-dir", help="Input directory.")
	parser.add_argument("-o", "--output-dir", help="Output directory.")
	parser.add_argument("-a", "--args", default="",
	                    help="Additional Arguments for HarryPlotter. [Default: %(default)s]")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	parser.add_argument("-f", "--n-plots", type=int,
	                    help="Number of plots. [Default: all]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	plot_configs = []
	plot_args = []
	for json_config in args.json_configs:
		plot_config = {} # config = jsonTools.JsonDict(os.path.expandvars(json_config)).doIncludes().doComments()
		plot_arg = args.args + " --json-defaults " + os.path.expandvars(json_config)
		
		if not args.input_dir is None:
			plot_config["directories"] = os.path.expandvars(args.input_dir)
		if not args.output_dir is None:
			plot_config["output_dir"] = os.path.expandvars(args.output_dir)
		
		plot_configs.append(plot_config)
		plot_args.append(plot_arg)
	
	harry_plotter = harry.HarryPlotter(list_of_config_dicts=plot_configs, list_of_args_strings=plot_args, n_processes=args.n_processes, n_plots=args.n_plots)

