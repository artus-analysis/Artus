#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.dcachetools as dcachetools


def main():
	
	parser = argparse.ArgumentParser(description="Read text files and convert dCache paths.", parents=[logger.loggingParser])
	
	parser.add_argument("input_files", nargs="+",
	                    help="Input text files.")
	parser.add_argument("-m", "--mode", default="xrd2dcap",
	                    choices=["xrd2srm", "srm2xrd", "srm2dcap", "dcap2srm", "dcap2xrd", "xrd2dcap"],
	                    help="Conversion mode. [Default: %(default)s]")
	parser.add_argument("-i", "--in-place", default=False, action="store_true",
	                    help="Convert paths in place modifying the input files. [Default: Only print converted files]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	for input_filename in args.input_files:
		with open(input_filename, "rw" if args.in_place else "r") as input_file:
			content = input_file.read()
			content = getattr(dcachetools, args.mode)(content)
			if args.in_place:
				input_file.write(content)
			else:
				log.info(content)


if __name__ == "__main__":
	main()

