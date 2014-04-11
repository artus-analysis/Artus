# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


class Processor(object):
	def __init__(self):
		pass
	
	def modify_argument_parser(self, parser):
		pass
	
	def run(self, plotdict=None):
		log.debug("Running processor " + self.__class__.__name__ + "...")
	
	@classmethod
	def name(cls):
		return cls.__name__

