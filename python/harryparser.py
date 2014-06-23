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
		
		super(HarryParser, self).__init__(**kwargs)
		
		self.add_argument("-h", "--help", default=False, action="store_true",
		                  help="show this help message and exit")
		
		module_options = self.add_argument_group('Modules')
		module_options.add_argument("--input-module", default="InputRoot",
		                  help="Input Module. [Default: %(default)s]")
		module_options.add_argument("--analysis-modules", default=None, nargs="*",
		                  help="Analysis Modules. [Default: %(default)s]")
		module_options.add_argument("--plot-modules", default="PlotMpl", nargs="*",
		                  help="Plot Modules. [Default: %(default)s]")
		                  
		self._n_parse_args_calls = 0
	
	def parse_args(self, args=None, namespace=None):
		args = super(HarryParser, self).parse_args(args=args, namespace=namespace)
		logger.initLogger(args)
		self._n_parse_args_calls += 1
		if self._n_parse_args_calls > 1 and (args.help or len(sys.argv) <= 1):
			self.print_help()
			sys.exit(0)
		return args
	
	def parse_known_args(self, args=None, namespace=None):
		args, unknown_args = super(HarryParser, self).parse_known_args(args=args, namespace=namespace)
		logger.initLogger(args)
		self._n_parse_args_calls += 1
		if self._n_parse_args_calls > 1 and (args.help or len(sys.argv) <= 1):
			self.print_help()
			sys.exit(0)
		return args, unknown_args

