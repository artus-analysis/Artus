
# -*- coding: utf-8 -*-

import argparse
import logging

# http://docs.python.org/2/library/logging.html
# http://docs.python.org/2/howto/logging.html#logging-advanced-tutorial

loggingParser = argparse.ArgumentParser(add_help=False)

logginOptionsGroup = loggingParser.add_argument_group("Logging options")
logginOptionsGroup.add_argument("--log-level", default="debug",
                                choices=["debug", "info", "warning", "error", "critical"],
                                help="Log level. [Default: debug]")
logginOptionsGroup.add_argument("--log-file", help="Log file. [Default: None (output on console)]")

def initLogger(argParserArgs=None, name=""):

	logLevel = "debug"
	logFile = None

	if argParserArgs:
		logLevel = argParserArgs.log_level
		logFile = argParserArgs.log_file

	logging.basicConfig(level=getattr(logging, logLevel.upper()),# fileName=logFile,
	                    format="%(levelname)s: %(message)s")

	if logFile:
		loggingFileHandler = logging.FileHandler(logFile)
		logging.getLogger().addHandler(loggingFileHandler)
		logging.getLogger(__name__).info("Output will be logged to \"%s\"." % logFile)

