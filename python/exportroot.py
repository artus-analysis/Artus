# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase


class ExportRoot(plotbase.PlotBase):
	def __init__(self):
		plotbase.PlotBase.__init__(self)

