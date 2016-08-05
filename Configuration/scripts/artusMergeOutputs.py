#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import shlex

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.tools as tools
import Artus.Utility.progressiterator as pi
from Artus.Utility.tools import hadd2


def main():
	
	parser = argparse.ArgumentParser(description="Merge Artus outputs per nick name.", parents=[logger.loggingParser])

	parser.add_argument("project_dir", help="Artus Project directory containing the files \"output/*/*.root\" to merge in case there is an output dir, */*.root else", nargs="*")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	parser.add_argument("--output-dir", help="Directory to store merged files. Default: Same as first project_dir.")
	args = parser.parse_args()
	logger.initLogger(args)
	outputs_per_nick = {}
	for project_dir in args.project_dir:
		extra_path = "output/" if (os.path.isdir(os.path.join(project_dir, "output"))) else ""
		output_dirs = glob.glob(os.path.join(project_dir, extra_path+"*"))
		nick_names = [nick for nick in [output_dir[output_dir.rfind("/")+1:] for output_dir in output_dirs] if not ".tar.gz" in nick]
		for nick in nick_names:
			if not (nick in outputs_per_nick):
				outputs_per_nick[nick] = []
			outputs_per_nick[nick] = outputs_per_nick[nick] + glob.glob(os.path.join(project_dir, extra_path, nick, "*.root"))
	# drop potentially existing SvfitCaches from the filelist
	for nick, files in outputs_per_nick.iteritems():
		outputs_per_nick[nick] = [file for file in files if ("SvfitCache" not in file)]
	outputs_per_nick = {nick : files for nick, files in outputs_per_nick.iteritems() if len(files) > 0}
	
	hadd_arguments = []
	for nick_name, output_files in pi.ProgressIterator(outputs_per_nick.iteritems(),
	                                                   length=len(outputs_per_nick),
	                                                   description="Merging Artus outputs"):
		merged_dir = os.path.join(args.project_dir[0] if(args.output_dir == None) else args.output_dir, "merged", nick_name)
		if not os.path.exists(merged_dir):
			os.makedirs(merged_dir)
		hadd_arguments.append({"target_file": os.path.join(merged_dir, nick_name+".root"), "source_files": output_files, "hadd_args" : " -f ", "max_files" : 500})
		#commands.append("hadd.py -a \" -f\" -t %s \"%s\"" % (os.path.join(merged_dir, nick_name+".root"), " ".join(output_files)))
	
	tools.parallelize(hadd2, hadd_arguments, n_processes=args.n_processes)

if __name__ == "__main__":
	main()

