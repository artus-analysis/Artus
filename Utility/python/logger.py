
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

def initLogger(argParserArgs=None, name=""):

	logLevel = "debug"

	if argParserArgs: logLevel = argParserArgs.log_level

	logging.basicConfig(level=getattr(logging, logLevel.upper()),
	                    format="%(levelname)s: %(message)s")

