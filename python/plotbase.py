# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc

import Artus.HarryPlotter.processor as processor


class PlotBase(processor.Processor):
	def __init__(self):
		processor.Processor.__init__(self)
	
	def prepare_args(self, plotData):
		processor.Processor.prepare_args(self, plotData)
	
	def run(self, plotdict=None):
		processor.Processor.run(self, plotdict)
	
	@abc.abstractmethod
	def create_canvas(self):
		return
	
	@abc.abstractmethod
	def make_plots(self):
		return
	
	@abc.abstractmethod
	def modify_axes(self):
		return
	
	@abc.abstractmethod
	def add_texts(self):
		return

