#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse


def main():
	
	parser = argparse.ArgumentParser(description="Execute bash commands.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("commands", nargs="+",
	                    help="Bash commands to be executed.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	for command in args.commands:
		logger.subprocessCall(command, shell=True)


if __name__ == "__main__":
	main()

