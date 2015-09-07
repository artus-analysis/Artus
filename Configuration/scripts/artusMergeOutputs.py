#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.tools as tools
import Artus.Utility.progressiterator as pi


def _call_command(command):
	log.debug(command)
	logger.subprocessCall(command.split())


def main():
	
	parser = argparse.ArgumentParser(description="Merge Artus outputs per nick name.", parents=[logger.loggingParser])

	parser.add_argument("project_dir", help="Artus Project directory containing the files \"output/*/*.root\" to merge")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	output_dirs = glob.glob(os.path.join(args.project_dir, "output/*"))
	nick_names = [nick for nick in [output_dir[output_dir.rfind("/")+1:] for output_dir in output_dirs] if not ".tar.gz" in nick]
	outputs_per_nick = {nick : glob.glob(os.path.join(args.project_dir, "output", nick, "*.root")) for nick in nick_names}
	outputs_per_nick = {nick : files for nick, files in outputs_per_nick.iteritems() if len(files) > 0}
	
	commands = []
	for nick_name, output_files in pi.ProgressIterator(outputs_per_nick.iteritems(),
	                                                   length=len(outputs_per_nick),
	                                                   description="Merging Artus outputs"):
		merged_dir = os.path.join(args.project_dir, "merged", nick_name)
		if not os.path.exists(merged_dir):
			os.makedirs(merged_dir)
	
		commands.append("hadd -f %s %s" % (os.path.join(merged_dir, nick_name+".root"), " ".join(output_files)))
	
	tools.parallelize(_call_command, commands, n_processes=args.n_processes)

if __name__ == "__main__":
	main()

