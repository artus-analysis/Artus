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
	
	def run(self, plotData):
		processor.Processor.run(self, plotData)
		
		self.create_canvas(plotData)
		self.make_plots(plotData)
		self.modify_axes(plotData)
		self.add_texts(plotData)
		self.save_canvas(plotData)
	
	@abc.abstractmethod
	def create_canvas(self, plotData):
		pass
	
	@abc.abstractmethod
	def make_plots(self, plotData):
		pass
	
	@abc.abstractmethod
	def modify_axes(self, plotData):
		pass
	
	@abc.abstractmethod
	def add_texts(self, plotData):
		pass
	
	@abc.abstractmethod
	def save_canvas(self, plotData):
		pass

