#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os


def main():
	
	parser = argparse.ArgumentParser(description="Execute bash command.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("command", nargs="+",
	                    help="Bash command to be executed.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	logger.subprocessCall(os.path.expandvars(" ".join(args.command)), shell=True)


if __name__ == "__main__":
	main()

