#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.Utility.tools as tools

import argparse
import sys


def main():
	
	parser = argparse.ArgumentParser(description="Download remote file with timeout.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("remote", help="Remote source file.")
	parser.add_argument("local", help="Local destimation file.")
	parser.add_argument("--offset", type=int, default=30,
	                    help="Wait for at most n seconds (default: %(default)s)")
	parser.add_argument("--bandwidth", type=int, default=100,
	                    help="Require at least n kB/s bandwidth (default: %(default)s)")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	result = tools.download_remote_file(remote=args.remote, local=args.local, offset=args.offset, bandwidth=args.bandwidth)
	if not result:
		sys.exit(1)


if __name__ == "__main__":
	main()

