#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import subprocess

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Delete old personal HP caches.", parents=[logger.loggingParser])

	parser.add_argument("--agecutoff", default=90, type=int,
	                    help="Delete caches the are older then this number of days. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)

	total = int(subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -1 | wc -l", shell=True))
	log.info("Total files in cache folder: "+str(total))

	## I guessed that checking the age of the oldest file would be faster than going through all files with "find", but this didn't seem to be the case
	#oldestfile = subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -tr | sed -n '1p'", shell=True)
	#oldestfiletime = subprocess.check_output("stat -c %Z $HP_WORK_BASE_COMMON/caches/"+oldestfile, shell=True)
	#currenttime = subprocess.check_output("date +%s", shell=True)
	#oldestfileage = (int(currenttime)-int(oldestfiletime))/60/60/24 #Output in seconds, calculate to days
	#old = 0
	#if oldestfileage > agecutoff:
	old = int(subprocess.check_output("find $HP_WORK_BASE_COMMON/caches -mtime +"+str(args.agecutoff)+" -user ${USER} -type f -name '*.root' | wc -l", shell=True))
	log.info("Files older than "+str(args.agecutoff)+" days: "+str(old))
	log.info("Remove these files?")

	"""
	if old > total/2:
		midfileage = args.agecutoff
		log.info("There are plenty of old files. Remove these?")
	else:
		midfile = subprocess.check_output("ls $HP_WORK_BASE_COMMON/caches -t | sed -n '"+str(total/2)+"p'", shell=True)
		midfiletime = subprocess.check_output("stat -c %Z $HP_WORK_BASE_COMMON/caches/"+midfile, shell=True)
		currenttime = subprocess.check_output("date +%s", shell=True)
		midfileage = (int(currenttime)-int(midfiletime))/60/60/24 #Output in seconds, calculate to days
		log.info("There are many files that aren't too old. Remove the older half? (Files older than "+str(midfileage)+" days)")
	"""

	answer = raw_input("[y|N]: ")
	if answer in ["y","Y"]:
		log.info("Removing old files...")
		delete = subprocess.check_output("find $HP_WORK_BASE_COMMON/caches -mtime +"+str(args.agecutoff)+" -user ${USER} -type f -name '*.root' -delete", shell=True)
		log.info("Done!")

