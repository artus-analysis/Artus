
# -*- coding: utf-8 -*-

import argparse
import logging
import sys


# Custom formatter with level-dependent formats
class ArtusFormatter(logging.Formatter):

	def __init__(self, fmtDict):
		defaultDebugFmt = "%(levelname)s (%(filename)s.%(lineno)d): %(message)s"
		logging.Formatter.__init__(self, fmt=defaultDebugFmt)
		fmtDict[0] = defaultDebugFmt
		self._fmtItems = sorted(fmtDict.items(), key=lambda item: item[0])

	def format(self, record):
		for levelno, fmt in self._fmtItems:
			if record.levelno >= levelno:
				self._fmt = fmt
		return logging.Formatter.format(self, record)


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

	logging.basicConfig(level=getattr(logging, logLevel.upper()))

	loggingHandler = logging.StreamHandler(sys.stdout)
	if logFile:
		loggingHandler = logging.FileHandler(logFile)
		logging.getLogger(__name__).info("Output will be logged to \"%s\"." % logFile)
	
	artusFormatter = ArtusFormatter({logging.DEBUG   : "%(levelname)s (%(filename)s.%(lineno)d): %(message)s",
	                                 logging.INFO    : "%(message)s",
	                                 logging.WARNING : "%(levelname)s: %(message)s"})
	loggingHandler.setFormatter(artusFormatter)
	logging.getLogger().addHandler(loggingHandler)

