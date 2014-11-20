#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import getpass
import re
import subprocess


def main():
	
	parser = argparse.ArgumentParser(description="Print qstat summary table.", parents=[logger.loggingParser])
	
	parser.add_argument("-s", "--sort-state", default="r",
	                    help="State to be used for sorting. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	# call qstat
	qstat_command = "qstat -u '*' -s prs"
	qstat_output = subprocess.Popen(qstat_command, stdout=subprocess.PIPE, shell=True).communicate()[0].split("\n")[2:]
	qstat_output = [job for job in qstat_output if job != ""]
	
	# parse output
	parsed_qstat_output = {}
	for job in qstat_output:
		parsed_job = re.match("(?P<jobid>\d+)\s+"+
		                      "(?P<prior>\d+.?\d*)\s+"+
		                      "(?P<name>\S+)\s+"+
		                      "(?P<user>\S+)\s+"+
		                      "(?P<state>\S+)\s+"+
		                      "(?P<start>\d\d/\d\d/\d\d\d\d\s\d\d:\d\d:\d\d)"+
		                      ".*", job).groupdict()
		parsed_qstat_output.setdefault(parsed_job["user"], {}).setdefault(parsed_job["state"], []).append(parsed_job["jobid"])
		parsed_qstat_output.setdefault(parsed_job["user"], {}).setdefault("all", []).append(parsed_job["jobid"])
	
	# check states
	states = list(set([state for states in [state_jobids.keys() for state_jobids in parsed_qstat_output.values()] for state in states]))
	sort_state = args.sort_state if args.sort_state in states else states[-1]
	#states.sort(cmp=lambda s1, s2: -1 if sort_state in (s1, s2) else (+1 if "all" in (s1, s2) else cmp(s1, s2)))
	states.sort()
	states.append(states.pop(states.index("all")))
	states.insert(0, states.pop(states.index(sort_state)))
	
	# check users
	users = sorted(parsed_qstat_output.keys(), reverse=True,
	               key=lambda user: len(parsed_qstat_output[user].get(sort_state, [])))
	max_len_users = max([len(user) for user in users])
	
	# format table header
	qstat_table = ((" \033[0;1m%" + ("%d" % max_len_users) + "s\033[0m") % "User")
	for state in states:
		qstat_table += (" | \033[0;1m%6s\033[0m" % state)
	qstat_table += ("\n" + ("-" * (max_len_users+2)) + (("|" + ("-"*8)) * len(states)))
	
	# format table body
	for user in users:
		line = ((" %" + ("%d" % max_len_users) + "s") % user)
		for state in states:
			line += (" | %6d" % len(parsed_qstat_output[user].get(state, [])))
		qstat_table += ("\n" + (("\033[0;31;47m"+line+"\033[0m") if user == getpass.getuser() else line))
	
	# output table
	log.info(qstat_table)


if __name__ == "__main__":
	main()

