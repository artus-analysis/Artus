#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.Utility.tools as tools

import argparse
import getpass
import math
import re
import subprocess
import sys
import time


def progress_bar(n_running, n_all, n_norm, size, this_user=False):
	running_char = "\033[0;42m"+(">" if this_user else " ")+"\033[0m"
	queued_char = "\033[0;43m"+("<" if this_user else " ")+"\033[0m"
	free_char = " "
	n_running_chars = int(math.ceil(float(n_running) * size / n_norm))
	n_queued_chars = min(int(math.ceil(float(n_all-n_running) * size / n_norm)), size - n_running_chars)
	n_free_chars = size - n_running_chars - n_queued_chars
	return "%s%s%s" % (running_char*n_running_chars, queued_char*n_queued_chars, free_char*n_free_chars)


def condor_state_string(state):
	# http://research.cs.wisc.edu/htcondor/manual/v7.6/10_Appendix_A.html
	return {
		"1" : "Idle",
		"2" : "Running",
		"3" : "Removed",
		"4" : "Completed",
		"5" : "Held",
		"6" : "Transferring Output",
	}.get(state, state)
	


def print_stats_table(args, previous_users=[]):

	# call stats
	stats_command = "qstat -u '*' -s prs" if args.qstat else "condor_q -global -allusers -autoformat ClusterId Owner JobStatus"
	stats_output = subprocess.Popen(stats_command, stdout=subprocess.PIPE, shell=True).communicate()[0].split("\n")[2:]
	stats_output = [job for job in stats_output if job != ""]
	
	stats_regex = ""
	if args.qstat:
		stats_regex = ("\s*(?P<jobid>\d+)\s+"+
		               "(?P<prior>\d+.?\d*)\s+"+
		               "(?P<name>\S+)\s+"+
		               "(?P<user>\S+)\s+"+
		               "(?P<state>\S+)\s+"+
		               "(?P<start>\d\d/\d\d/\d\d\d\d\s\d\d:\d\d:\d\d)"+
		               ".*")
	else:
		stats_regex = ("\s*(?P<jobid>\d+)\s+"+
		               "(?P<user>\S+)\s+"+
		               "(?P<state>\d+)"+
		               ".*")
	
	# parse output
	parsed_stats_output = {}
	for job in stats_output:
		parsed_job = re.match(stats_regex, job).groupdict()
		state = parsed_job["state"]
		if not args.qstat:
			state = condor_state_string(state)
		parsed_stats_output.setdefault(parsed_job["user"], {}).setdefault(state, []).append(parsed_job["jobid"])
		parsed_stats_output.setdefault(parsed_job["user"], {}).setdefault("all", []).append(parsed_job["jobid"])

	# check states
	states = list(set([state for states in [state_jobids.keys() for state_jobids in parsed_stats_output.values()] for state in states if (not args.simple_mode or state in ['all', 'r'])]))
	sort_state = args.sort_state if args.sort_state in states else states[-1]
	#states.sort(cmp=lambda s1, s2: -1 if sort_state in (s1, s2) else (+1 if "all" in (s1, s2) else cmp(s1, s2)))
	states.sort()
	states.append(states.pop(states.index("all")))
	states.insert(0, states.pop(states.index(sort_state)))

	# check users
	users = sorted(parsed_stats_output.keys(), reverse=True,
	               key=lambda user: len(parsed_stats_output[user].get(sort_state, [])))
	max_len_users = max([len(user) for user in users])
	tty_width = tools.get_tty_size()[1]

	# get a list of up/down arrow symbols by comparing rank to previous rank
	symbols = []
	for rank, user in enumerate(users):
		# get previous rank
		if user in previous_users:
			previous_rank = previous_users.index(user)
		else:
			previous_rank = -1
		# get symbol according to rank - previous-rank comparison
		if previous_rank == -1:
			symbols += [" "]
		elif rank<previous_rank:
			symbols += ["\033[1;32m%s\033[0m" % u"\u2191".encode('utf8')]
		elif rank>previous_rank:
			symbols += ["\033[1;31m%s\033[0m" % u"\u2193".encode('utf8')]
		else:
			symbols += [" "]

	n_total_jobs = sum([len(state_jobids.get("all", [])) for state_jobids in parsed_stats_output.values()])
	max_n_jobs = max([len(state_jobids.get("all", [])) for state_jobids in parsed_stats_output.values()])
	max_n_jobs_len = max(len(str(max_n_jobs)), max([len(s) for s in states]))

	# get length of maximum number of jobs per state (to adjust column width)
	length_dict = {}
	for state in states:
		length_dict[state] = max([len(str(max([len(state_jobids.get(state, [])) for state_jobids in parsed_stats_output.values()]))), len(state)])

	# format table header
	stats_table = ((" \033[0;1m%" + ("%d" % (max_len_users + 1)) + "s\033[0m | ") % "User")
	second_line = ("-" * (max_len_users+3))
	for state in states:
		stats_table += ("\033[0;1m%*s\033[0m | " % (length_dict[state] ,state))
		second_line += "|" + "-" * (length_dict[state] +2)
	second_line += "|"+ ("-" * (tty_width-len(second_line)-1))
	stats_table += ("\n" + second_line)

	# format table body
	for user, symbol in zip(users, symbols):
		line = (("%" + ("%*d" % (max_n_jobs_len, max_len_users)) + "s | ") % user)
		for state in states:
			line += ("%*d | " % (length_dict[state], len(parsed_stats_output[user].get(state, []))) )
		line += progress_bar(len(parsed_stats_output[user].get("r", parsed_stats_output[user].get("Running", []))),
		                     len(parsed_stats_output[user].get("all", [])),
		                     n_total_jobs if args.norm_total_jobs else max_n_jobs,
		                     tty_width-len(line)-2,
		                     this_user=(user == getpass.getuser()))
		stats_table += ("\n" + symbol + " " + (("\033[0;31;47m"+line+"\033[0m") if user == getpass.getuser() else line))

	# output table
	if args.continuous_mode != 0:
		sys.stdout.write("\033[?1049h\033[2J") # switch to alternative buffer, like e.g. screen does
	sys.stdout.write(stats_table)
	sys.stdout.flush()
	return users


def main():

	parser = argparse.ArgumentParser(description="Print stats summary table.", parents=[logger.loggingParser])

	batch_system_group = parser.add_mutually_exclusive_group()
	batch_system_group.add_argument("--qstat", dest="qstat", default=True, action="store_true")
	batch_system_group.add_argument("--condor", dest="qstat", default=True, action="store_false")
	
	parser.add_argument("-s", "--sort-state", default="r",
	                    help="State to be used for sorting. [Default: %(default)s]")
	parser.add_argument("--norm-total-jobs", default=False, action="store_true",
	                    help="Norm progress bar to total number of jobs in batch system. [Default: Norm to user with most job in system.]")
	parser.add_argument("-S", "--simple-mode", default=False, action="store_true",
	                    help="Simple mode. Only display all and running jobs.")
	parser.add_argument("-c", "--continuous-mode", default=0, type=int,
	                    help="Continuous mode. Argument is refresh interval. Default is non-continuous mode.")

	args = parser.parse_args()
	logger.initLogger(args)

	users = []
	while True:
		try:
			users = print_stats_table(args, users)
			if args.continuous_mode == 0:
				break
			time.sleep(args.continuous_mode)
		except:
			sys.stdout.write("\033[?1049l") # switch back to default buffer
			sys.stdout.flush()
			sys.exit(0)


if __name__ == "__main__":
	main()

