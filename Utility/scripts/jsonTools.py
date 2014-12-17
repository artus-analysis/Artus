#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.jsonTools as jsonTools


def main():
	
	parser = argparse.ArgumentParser(description="Tools for JSON files.", parents=[logger.loggingParser])
	
	parser.add_argument("json", nargs="+", help="JSON configs.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	log.info(jsonTools.JsonDict(args.json).doIncludes().doComments())

if __name__ == "__main__":
	main()

