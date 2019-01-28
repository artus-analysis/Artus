#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import re

import Artus.HarryPlotter.utility.roottools as roottools
from Artus.Utility.tfilecontextmanager import TFileContextManager


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Print content of ROOT file", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")
	parser.add_argument("-e", "--elements", nargs="+", help="Regexes for elements for which the code should be executed.", default=[])
	parser.add_argument("-c", "--codes", nargs="+", help="Codes to be executed for matching elements. \"element\" is replaced by the matching element.", default=[])

	args = parser.parse_args()
	logger.initLogger(args)
	
	with TFileContextManager(args.root_file, "READ") as root_file:
		elements = roottools.RootTools.walk_root_directory(root_file)
		for index, (key, path) in enumerate(elements):
			class_name = key.GetClassName()
			log.info("%s (%s)" % (path, class_name))
			for regex, code in zip(args.elements, args.codes):
				if re.match(regex, path):
					root_object = root_file.Get(path)
					result = eval(code.replace("element", "root_object"))
					if result:
						log.info(code.replace("element", path))
						log.info(result)
					if index < len(elements)-1:
						log.info("\n" + (100*"-") + "\n")
			if log.isEnabledFor(logging.DEBUG):
				root_object = root_file.Get(path)
				log.debug("")
				root_object.Print("")
				if index < len(elements)-1:
					log.debug("\n" + (100*"=") + "\n")
				"""
				if (class_name == "TTree") or (class_name == "TNtuple"):
					log.debug("%s (%s, entries=%d)" % (path, class_name, root_object.GetEntries()))
				elif class_name.startswith("TH") or class_name.startswith("TProfile"):
					log.debug("%s (%s, integral=%f)" % (path, class_name, root_object.Integral()))
				elif class_name.startswith("TGraph"):
					log.debug("%s (%s, points=%d)" % (path, class_name, root_object.GetN()))
				"""

