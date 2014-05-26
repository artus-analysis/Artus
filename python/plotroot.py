# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import ROOT
import sys

import Artus.HarryPlotter.plotbase as plotbase

ROOT.PyConfig.IgnoreCommandLineOptions = True


class PlotRoot(plotbase.PlotBase):
	def __init__(self):
		plotbase.PlotBase.__init__(self)
		
		self.canvas = None
		self.plot_pad = None
   		self.ratio_pad = None
	
	def prepare_args(self, plotData):
		plotbase.PlotBase.prepare_args(self, plotData)
	
	def run(self, plotData):
		plotbase.PlotBase.run(self, plotData)

	def create_canvas(self, plotData):
		if self.canvas == None:
			self.canvas = ROOT.TCanvas()
		
		if plotData.plotdict["ratio"]:
			plot_ratio_slider_y = 0.35
			self.canvas.cd()
			if self.plot_pad == None:
				self.plot_pad = ROOT.TPad("plot_pad", "", 0.0, plot_ratio_slider_y, 1.0, 1.0)
				self.plot_pad.SetNumber(1)
				self.plot_pad.Draw()
			if self.ratio_pad == None:
		   		self.ratio_pad = ROOT.TPad("ratio_pad", "", 0.0, 0.0, 1.0, plot_ratio_slider_y)
				self.ratio_pad.SetNumber(2)
				self.ratio_pad.Draw()
			
			self.plot_pad.SetBottomMargin(0.02);
		
		else:
			self.plot_pad = self.canvas
			

	def prepare_histograms(self, plotData):
		pass
	
	def make_plots(self, plotData):
	
		self.plot_pad.cd()
		
		self.x_min = sys.float_info.max
		self.x_max = -sys.float_info.max
		self.y_min = sys.float_info.max
		self.y_max = -sys.float_info.max
		self.z_min = sys.float_info.max
		self.z_max = -sys.float_info.max
		
		for index, root_histogram in enumerate(plotData.plotdict["root_histos"].values()):
			option = "" if index == 0 else "same"
			root_histogram.Draw(option)
			if index == 0: self.first_plotted_histogram = root_histogram
			
			# trace min. and max. values of axes
			if root_histogram.GetXaxis().GetXmin() < self.x_min: self.x_min = root_histogram.GetXaxis().GetXmin()
			if root_histogram.GetXaxis().GetXmax() > self.x_max: self.x_max = root_histogram.GetXaxis().GetXmax()
			if root_histogram.GetDimension() > 1:
				if root_histogram.GetYaxis().GetXmin() < self.y_min: self.y_min = root_histogram.GetYaxis().GetXmin()
				if root_histogram.GetYaxis().GetXmax() > self.y_max: self.y_max = root_histogram.GetYaxis().GetXmax()
			else:
				if root_histogram.GetMinimum() < self.y_min: self.y_min = root_histogram.GetMinimum()
				if root_histogram.GetMaximum() > self.y_max: self.y_max = root_histogram.GetMaximum()
			if root_histogram.GetDimension() > 2:
				if root_histogram.GetZaxis().GetXmin() < self.z_min: self.z_min = root_histogram.GetZaxis().GetXmin()
				if root_histogram.GetZaxis().GetXmax() > self.z_max: self.z_max = root_histogram.GetZaxis().GetXmax()
			else:
				if root_histogram.GetMinimum() < self.z_min: self.z_min = root_histogram.GetMinimum()
				if root_histogram.GetMaximum() > self.z_max: self.z_max = root_histogram.GetMaximum()
		
		if plotData.plotdict["ratio"]:
			self.ratio_pad.cd()
			
			self.ratio_y_min = sys.float_info.max
			self.ratio_y_max = -sys.float_info.max
		
			for index, root_histogram in enumerate(plotData.plotdict["root_ratio_histos"]):
				option = "" if index == 0 else "same"
				root_histogram.Draw(option)
				if index == 0: self.first_plotted_ratio_histogram = root_histogram
			
				# trace min. and max. values of axes
				if root_histogram.GetMinimum() < self.ratio_y_min: self.ratio_y_min = root_histogram.GetMinimum()
				if root_histogram.GetMaximum() > self.ratio_y_max: self.ratio_y_max = root_histogram.GetMaximum()
			
	
	def modify_axes(self, plotData):
	
		# axis labels
		self.first_plotted_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
		self.first_plotted_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
		self.first_plotted_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])
		if plotData.plotdict["ratio"]:
			self.first_plotted_ratio_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			self.first_plotted_ratio_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
			self.first_plotted_ratio_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])
	
		# logaritmic axis
		if plotData.plotdict["x_log"]: self.plot_pad.SetLogx()
		if plotData.plotdict["y_log"]: self.plot_pad.SetLogy()
		if plotData.plotdict["z_log"]: self.plot_pad.SetLogz()
		if plotData.plotdict["ratio"]:
			if plotData.plotdict["x_log"]: self.ratio_pad.SetLogx()
	
		# axis limits
		if plotData.plotdict["x_lims"] == None:
			self.first_plotted_histogram.GetXaxis().SetRangeUser(self.x_min,
			                                                     self.x_max)
			if plotData.plotdict["ratio"]:
				self.first_plotted_ratio_histogram.GetXaxis().SetRangeUser(self.x_min,
					                                                       self.x_max)
		else:
			self.first_plotted_histogram.GetXaxis().SetRangeUser(plotData.plotdict["x_lims"][0],
			                                                     plotData.plotdict["x_lims"][1])
			if plotData.plotdict["ratio"]:
				self.first_plotted_ratio_histogram.GetXaxis().SetRangeUser(plotData.plotdict["x_lims"][0],
					                                                       plotData.plotdict["x_lims"][1])
		if plotData.plotdict["y_lims"] == None:
			self.first_plotted_histogram.GetYaxis().SetRangeUser(self.y_min,
			                                                     self.y_max)
		else:
			self.first_plotted_histogram.GetYaxis().SetRangeUser(plotData.plotdict["y_lims"][0],
			                                                     plotData.plotdict["y_lims"][1])
		if plotData.plotdict["ratio"]:
			if plotData.plotdict["y_ratio_lims"] == None:
				self.first_plotted_ratio_histogram.GetYaxis().SetRangeUser(self.ratio_y_min,
					                                                       self.ratio_y_max)
			else:
				self.first_plotted_ratio_histogram.GetYaxis().SetRangeUser(plotData.plotdict["y_ratio_lims"][0],
					                                                       plotData.plotdict["y_ratio_lims"][1])
		if plotData.plotdict["z_lims"] == None:
			self.first_plotted_histogram.GetZaxis().SetRangeUser(self.z_min,
			                                                     self.z_max)
		else:
			self.first_plotted_histogram.GetZaxis().SetRangeUser(plotData.plotdict["z_lims"][0],
			                                                     plotData.plotdict["z_lims"][1])
		
		if plotData.plotdict["ratio"]:
			   self.first_plotted_histogram.GetXaxis().SetLabelSize(0)
			   self.first_plotted_histogram.GetXaxis().SetTitleSize(0)
		
	def add_labels(self, plotData):
		pass
	
	def add_texts(self, plotData):
		pass
	
	def save_canvas(self, plotData):
		for plot_format in plotData.plotdict["formats"]:
			filename = os.path.join(plotData.plotdict["output_dir"],
			                        plotData.plotdict["filename"]+"."+plot_format)
			self.canvas.SaveAs(filename)
			log.info("Created plot \"%s\"." % filename)

