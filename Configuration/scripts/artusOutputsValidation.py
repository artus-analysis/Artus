#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import gzip
import os
import re
import shlex

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.HarryPlotter.utility.roottools as roottools
import Artus.Utility.tfilecontextmanager as tfilecontextmanager
import Artus.Utility.progressiterator as progressiterator


def main():
	
	parser = argparse.ArgumentParser(description="Check the validity of Artus outputs in a project directory.", parents=[logger.loggingParser])
	
	parser.add_argument("project_directory", help="Artus project directory")
	parser.add_argument("--dry-run", help="Only print invalid job numbers", default=False, action="store_true")

	args = parser.parse_args()
	logger.initLogger(args)
	
	# GC settings
	artus_gc_config = os.path.join(args.project_directory, "grid-control_config.conf")
	
	n_artus_jobs = 0
	with gzip.open(os.path.join(args.project_directory, "workdir/params.map.gz"), "rb") as gc_params_map:
		n_artus_jobs = int(gc_params_map.read().strip().rstrip())
	
	# locate artus outputs	
	artus_outputs = {}
	
	artus_root_outputs = glob.glob(os.path.join(args.project_directory, "output/*/*_job_*_output.root"))
	for artus_root_output in progressiterator.ProgressIterator(artus_root_outputs, description="Locating Artus ROOT output"):
		artus_outputs.setdefault(
				int(re.search(".*_job_(?P<job_number>\d+)_output.root", artus_root_output).groupdict().get("job_number")),
				{}
		)["root"] = artus_root_output
	
	artus_log_outputs = glob.glob(os.path.join(args.project_directory, "output/*/*_job_*_log.log"))
	for artus_log_output in progressiterator.ProgressIterator(artus_log_outputs, description="Locating Artus log output"):
		artus_outputs.setdefault(
				int(re.search(".*_job_(?P<job_number>\d+)_log.log", artus_log_output).groupdict().get("job_number")),
				{}
		)["log"] = artus_log_output
	
	failed_job_numbers = []
	
	# check existance of all files
	for job_number in progressiterator.ProgressIterator(xrange(n_artus_jobs), description="Check existance of all Artus outputs"):
		if ((artus_outputs.get(job_number) is None) or
		    (artus_outputs.get(job_number).get("root") is None) or
		    (artus_outputs.get(job_number).get("log") is None)):
			
			failed_job_numbers.append(str(job_number))
	
	# check validity of ROOT files
	for job_number, outputs in progressiterator.ProgressIterator(artus_outputs.items(), description="Check validity of Artus ROOT outputs"):
		with tfilecontextmanager.TFileContextManager(outputs["root"], "READ") as root_file:
			# https://root.cern.ch/root/roottalk/roottalk02/4340.html
			if root_file.IsZombie() or root_file.TestBit(ROOT.TFile.kRecovered):
				failed_job_numbers.append(str(job_number))
			else:
				elements = roottools.RootTools.walk_root_directory(root_file)
				if len(elements) <= 1:
					failed_job_numbers.append(str(job_number))
	
	gc_reset_command = "go.py --reset id:"+(",".join(failed_job_numbers))+" "+artus_gc_config
	log.info(gc_reset_command)
	
	if not args.dry_run:
		logger.subprocessCall(shlex.split(gc_reset_command))
		
		gc_run_command = "go.py "+artus_gc_config
		log.info(gc_run_command)
		logger.subprocessCall(shlex.split(gc_run_command))
	

if __name__ == "__main__":
	main()

