#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import ROOT

import Artus.Utility.jsonTools as jsonTools
import Artus.HarryPlotter.harry as harry


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Plot runtimes of Artus processors. Requires running the RunTimeConsumer in Artus.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("-i", "--input", help="Artus output file containing trees from the RunTimeConsumer.")
	parser.add_argument("-p", "--pipeline", help="Artus pipeline. The corresponding folder must contain a tree named runTime.")
	parser.add_argument("-o", "--output-dir", help="Output directory.")
	parser.add_argument("-a", "--args", default="",
	                    help="Additional Arguments for HarryPlotter. [Default: %(default)s]")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	parser.add_argument("-f", "--n-plots", type=int,
	                    help="Number of plots. [Default: all]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	folder = args.pipeline+"/runTime"
	
	# retrieve list of processors
	runtime_tree = ROOT.TChain(folder)
	runtime_tree.Add(args.input)
	processors = [leaf.GetName() for leaf in runtime_tree.GetListOfLeaves()]
	
	# plot all runtimes
	plot_configs = []
	for processor in processors:
		plot_config = {}
		plot_config["files"] = [args.input]
		plot_config["folders"] = [folder]
		plot_config["x_expressions"] = [processor]
		plot_config["weights"] = ["{p} >= 0.0".format(p=processor)]
		plot_config["colors"] = ["#FF0000"]
		plot_config["x_label"] = "runtime / us"
		plot_config["filename"] = processor
		
		if not args.output_dir is None:
			plot_config["output_dir"] = os.path.expandvars(args.output_dir)
		
		plot_configs.append(plot_config)
	
	plot_config_combined = {}
	plot_config_combined["files"] = [args.input]
	plot_config_combined["folders"] = [folder]
	plot_config_combined["nicks"] = ["nick"]
	plot_config_combined["x_expressions"] = ["{i}".format(i=index) for index in range(len(processors))]
	plot_config_combined["y_expressions"] = ["{p}".format(p=processor) for processor in processors]
	plot_config_combined["weights"] = ["{p} >= 0.0".format(p=processor) for processor in processors]
	plot_config_combined["x_bins"] = ["{l},0,{l}".format(l=len(processors))]
	#plot_config_combined["y_bins"] = ["500,0,500"]
	plot_config_combined["tree_draw_options"] = ["profs"]
	plot_config_combined["x_label"] = "Processor Index"
	plot_config_combined["y_label"] = "Runtime / us"
	plot_config_combined["filename"] = "all"
	plot_configs.append(plot_config_combined)
	
	harry_plotter = harry.HarryPlotter(list_of_config_dicts=plot_configs, list_of_args_strings=[args.args], n_processes=args.n_processes, n_plots=args.n_plots)

