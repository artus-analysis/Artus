#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import string
import subprocess


def main():
	
	parser = argparse.ArgumentParser(description="Merge Artus outputs per nick name.", parents=[logger.loggingParser])

	parser.add_argument("project_dir", help="Artus Project directory containing the files \"output/*/*.root\" to merge")

	args = parser.parse_args()
	logger.initLogger(args)
	
	gc_config_base_filename = os.path.expandvars("$CMSSW_BASE/src/Artus/Configuration/data/gc_merge_artus_outputs_base.conf")
	gc_config_base = ""
	with open(gc_config_base_filename) as gc_config_base_file:
		gc_config_base = gc_config_base_file.read().rstrip()
	
	hadd_path = subprocess.Popen(["which", "hadd"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[0]
	gc_config = string.Template(gc_config_base).safe_substitute(
			PROJECT_DIR=args.project_dir,
			CMSSW_BASE=os.path.expandvars("$CMSSW_BASE"),
			SCRAM_ARCH=os.path.expandvars("$SCRAM_ARCH"),
			HADD_PATH=hadd_path,
	)
	
	gc_config_filename = os.path.join(args.project_dir, "gc_merge_artus_outputs.conf")
	if os.path.exists(gc_config_filename):
		os.remove(gc_config_filename)
		logger.subprocessCall(("rm -rf " + os.path.join(args.project_dir, "workdir_merge")).split())
	with open(gc_config_filename, "w") as gc_config_file:
		gc_config_file.write(gc_config)
	
	command = "go.py " + gc_config_filename
	log.info(command)
	logger.subprocessCall(command.split())
	
	log.info("Output is written to directory \"%s\"" % os.path.join(args.project_dir, "merged"))


if __name__ == "__main__":
	main()

