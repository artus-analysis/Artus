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
	parser = argparse.ArgumentParser(description="Collect outputs of failed GC jobs.", parents=[logger.loggingParser])
	parser.add_argument("INPUT_DIR",
	                    help="Project directory of Artus GC run")
	parser.add_argument("-o", "--output-dir",
	                    help="Directory where to put symlinks to failed job outputs. [Default: INPUT_DIR/failed]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	input_dir = args.INPUT_DIR
	if args.output_dir == None:
		args.output_dir = os.path.join(input_dir, "failed")
	
	if not os.path.exists(args.output_dir):
		os.makedirs(args.output_dir)
	
	job_info_file_names = glob.glob(os.path.join(input_dir, "workdir/output/job_*/job.info"))
	job_infos = {}
	for job_info_file_name in job_info_file_names:
		with open(job_info_file_name) as job_info_file:
			job_info_content = re.search("JOBID=(?P<job_id>\d*).*EXITCODE=(?P<exit_code>\d*)",
			                             job_info_file.read().replace("\n", "")).groupdict()
			job_infos[int(job_info_content["job_id"])] = int(job_info_content["exit_code"])
	
	job_infos = {job_id : exit_code for job_id, exit_code in job_infos.iteritems() if exit_code != 0}

	for job_id, exit_code in job_infos.iteritems():
		output_dir = os.path.join(args.output_dir, "job_id_%d_exit_code_%d" % (job_id, exit_code))
		if not os.path.exists(output_dir):
			os.makedirs(output_dir)
		
		os.system("ln -vs %s %s" % (os.path.join(input_dir, "workdir/output/job_%d" % job_id),
		                            os.path.join(output_dir, "gc_job_%d" % job_id)))
		os.system("ln -vs %s %s" % (os.path.join(input_dir, "output/*/*_%d_*" % job_id), output_dir))

if __name__ == "__main__":
	main()

