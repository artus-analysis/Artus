# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.processor as PlotBase


class PlotRoot(processor.PlotBase):
	def __init__(self):
		processor.PlotBase.__init__(self)

