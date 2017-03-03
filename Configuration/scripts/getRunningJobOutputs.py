#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import re


def main():
	parser = argparse.ArgumentParser(description="Collect outputs of running GC jobs.", parents=[logger.loggingParser])
	parser.add_argument("INPUT_DIR",
	                    help="Project directory of Artus GC run")
	parser.add_argument("-o", "--output-dir",
	                    help="Directory where to put symlinks to failed job outputs. [Default: INPUT_DIR/failed]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	input_dir = args.INPUT_DIR
	if args.output_dir == None:
		args.output_dir = os.path.join(input_dir, "running")
	
	if not os.path.exists(args.output_dir):
		os.makedirs(args.output_dir)
	
	job_info_file_names = glob.glob(os.path.join(input_dir, "workdir/jobs/job_*.txt"))
	for job_info_file_name in job_info_file_names:
		job_id = re.search(os.path.join(input_dir, "workdir/jobs/job_(?P<job_id>\d*).txt"), job_info_file_name).groupdict()["job_id"]
		if not os.path.exists(os.path.join(input_dir, "workdir/output/job_"+job_id+"/job.info")):
		
			output_dir = os.path.join(args.output_dir, "job_id_"+job_id)
			if not os.path.exists(output_dir):
				os.makedirs(output_dir)
			
			os.system("ln -vs %s %s" % (os.path.join(input_dir, "workdir/jobs/job_"+job_id+".txt"), output_dir))
			os.system("ln -vs %s %s" % (os.path.join(input_dir, "workdir/jobs/job_"+job_id+".var"), output_dir))

if __name__ == "__main__":
	main()

