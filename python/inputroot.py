# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import Artus.HarryPlotter.inputbase as inputbase


class InputRoot(inputbase.InputBase):
	def __init__(self):
		inputbase.InputBase.__init__(self)
	
	def modify_argument_parser(self, parser):
		inputbase.InputBase.modify_argument_parser(self, parser)
		self.input_options.add_argument("--root-objects", type=str, nargs='*',
		                                help="Path(s) to ROOT objects")
	
	def run(self, plotdict=None):
		inputbase.InputBase.run(self, plotdict)

