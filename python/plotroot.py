# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.plotbase as plotbase


class PlotRoot(plotbase.PlotBase):
	def __init__(self):
		plotbase.PlotBase.__init__(self)
	
	def prepare_args(self, plotData):
		plotbase.PlotBase.prepare_args(self, plotData)
	
	def run(self, plotData):
		plotbase.PlotBase.run(self, plotData)

	def create_canvas(self, plotData):
		self.canvas = ROOT.TCanvas()
	
	def make_plots(self, plotData):
		self.canvas.cd()
		for index, root_histogram in enumerate(plotData.plotdict["root_histos"].values()):
			option = "" if index == 0 else "same"
			root_histogram.Draw(option)
	
	def modify_axes(self, plotData):
		pass
	
	def add_texts(self, plotData):
		pass
	
	def save_canvas(self, plotData):
		self.canvas.SaveAs("test.png")

