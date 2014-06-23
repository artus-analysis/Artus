# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase


class PlotMpl(plotbase.PlotBase):
	def __init__(self):
		super(PlotMpl, self).__init__()
	
	def prepare_args(self, plotData):
		super(PlotMpl, self).prepare_args(plotData)
		
		# defaults for colors
		for index, color in enumerate(plotData.plotdict["colors"]):
			pass # TODO
		self.set_default_ratio_colors(plotData)
	
	def run(self, plotData):
		super(PlotMpl, self).run(plotData)

