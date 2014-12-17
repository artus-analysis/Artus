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
		
		self.module_options = self.add_argument_group('Modules')
		self.module_options.add_argument("--modules-search-paths", default=[], nargs="+",
		                                 help="Additional paths to be searched for modules.")
		self.module_options.add_argument("--input-module", default="InputRoot",
		                                 help="Input Module. [Default: %(default)s]")
		self.module_options.add_argument("--analysis-modules", default=[], nargs="+",
		                                 help="Analysis Modules. [Default: %(default)s]")
		self.module_options.add_argument("--plot-modules", default="PlotMpl", nargs="+",
		                                 help="Plot Modules. [Default: %(default)s]")
		self.module_options.add_argument("--list-available-modules", default=False, action="store_true",
		                                 help="List all available modules.")

		
		self.json_options = self.add_argument_group('JSON Configs')
		self.json_options.add_argument("-j", "--json-defaults", nargs="+",
		                               help="JSON config file(s) containing default settings (for a certain plot).")
		self.json_options.add_argument("--export-json", default=None, nargs='?', const='',
		                               help="Export arguments to specified JSON config file. If plotting from an input JSON config, giving no argument just updates this input JSON.")

		# Register new keyword 'bool' for parser
		self.register('type','bool',self._str2bool) 

	@staticmethod
	def _str2bool(v):
		""" Parse string content to bool."""
		return v.lower() in ("yes", "true", "t", "1")
	
	def parse_args(self, args=None, namespace=None):
		known_args = super(HarryParser, self).parse_args(args=args, namespace=namespace)
		logger.initLogger(known_args)

		# Add help after first parsing
		self.add_help = True
		self.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help='show this help message and exit')

		return known_args
	
	def parse_known_args(self, args=None, namespace=None):
		known_args, unknown_args = super(HarryParser, self).parse_known_args(args=args, namespace=namespace)
		logger.initLogger(known_args)

		# Add help after first parsing
		self.add_help = True
		self.add_argument('-h', '--help', action='help', default=argparse.SUPPRESS, help='show this help message and exit')

		return known_args, unknown_args

