# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import sys


class HarryParser(argparse.ArgumentParser):
	def __init__(self, **kwargs):
		kwargs["add_help"] = False
		kwargs["conflict_handler"] = "resolve"
		kwargs["fromfile_prefix_chars"] = "@"
		kwargs.setdefault("parents", []).append(logger.loggingParser)
		
		argparse.ArgumentParser.__init__(self, **kwargs)
		
		self.add_argument("-h", "--help", default=False, action="store_true",
		                  help="show this help message and exit")
		
		self.add_argument("--input-module", default="InputRoot",
		                  help="Input Module. [Default: %(default)s]")
		self.add_argument("--analysis-modules", default=None, nargs="*",
		                  help="Analysis Modules. [Default: %(default)s]")
		self.add_argument("--plot-modules", default="PlotMpl", nargs="*",
		                  help="Plot Modules. [Default: %(default)s]")
		                  
		self._n_parse_args_calls = 0
	
	def parse_args(self, args=None, namespace=None):
		args = argparse.ArgumentParser.parse_args(self, args=args, namespace=namespace)
		logger.initLogger(args)
		self._n_parse_args_calls += 1
		if args.help and self._n_parse_args_calls > 1:
			self.print_help()
			sys.exit(0)
		return args

