# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase


class ExportRoot(plotbase.PlotBase):
	def __init__(self):
		super(ExportRoot, self).__init__()
	
	def prepare_args(self, plotData):
		super(ExportRoot, self).prepare_args(plotData)
	
	def run(self, plotdict=None):
		super(ExportRoot, self).run(plotdict)

