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
	
	log_file_names = glob.glob(os.path.join(input_dir, "output/*/*_job_*_log.txt"))
	log_file_name_pattern = os.path.join(input_dir, "output/(?P<nick>.*)/.*_job_(?P<job_id>\d*)_log.txt")
	job_infos = {}
	for log_file_name in log_file_names:
		job_running = False
		with open(log_file_name) as log_file:
			job_running = "Output file \"output.root\" closed." not in log_file.read()
		
		if job_running:
			job_info = re.search(log_file_name_pattern, log_file_name).groupdict()
			job_infos.setdefault(job_info["nick"], []).append(int(job_info["job_id"]))

	for nick, job_ids in job_infos.iteritems():
		for job_id in job_ids:
			output_dir = os.path.join(args.output_dir, "nick_%s_job_id_%d" % (nick, job_id))
			if not os.path.exists(output_dir):
				os.makedirs(output_dir)
			
			os.system("ln -vs %s %s" % (os.path.join(input_dir, "workdir/output/job_%d" % job_id),
				                        os.path.join(output_dir, "gc_job_%d" % job_id)))
			os.system("ln -vs %s %s" % (os.path.join(input_dir, "output/*/*_%d_*" % job_id), output_dir))
	
	log.info("")
	for nick, job_ids in job_infos.iteritems():
		log.info(nick + ": " + str(len(job_ids)) + " jobs running")

if __name__ == "__main__":
	main()

