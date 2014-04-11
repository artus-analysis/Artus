# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.processor as processor


class AnalysisBase(processor.Processor):
	def __init__(self):
		processor.Processor.__init__(self)
	
	def run(self, plotdict=None):
		processor.Processor.run(self, plotdict)

