#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.jsonTools as jsonTools



def main():
	
	parser = argparse.ArgumentParser(description="Print names of all pipelines from Artus JSON config", parents=[logger.loggingParser])
	
	parser.add_argument("config", help="Artus JSON config. Can be either Artus output root files or JSON text files")

	args = parser.parse_args()
	logger.initLogger(args)
	
	config = jsonTools.JsonDict(args.config)
	log.info("\n".join(sorted(config.get("Pipelines", {}).keys())))

if __name__ == "__main__":
	main()

