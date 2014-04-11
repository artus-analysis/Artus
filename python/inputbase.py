# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc

import Artus.HarryPlotter.processor as Processor


class InputBase(processor.Processor):
	def __init__(self):
		processor.Processor.__init__(self)
	
	def run(self):
		processor.Processor.run(self)

	@abc.abstractmethod
	def read_root_objects(self):
		return

