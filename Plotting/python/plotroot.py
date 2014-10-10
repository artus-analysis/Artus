# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import ROOT
import sys

import HarryPlotter.Plotting.plotbase as plotbase

ROOT.PyConfig.IgnoreCommandLineOptions = True


class PlotRoot(plotbase.PlotBase):
	def __init__(self):
		super(PlotRoot, self).__init__()
		
		self.canvas = None
		self.plot_pad = None
   		self.ratio_pad = None
	
	def modify_argument_parser(self, parser, args):
		super(PlotRoot, self).modify_argument_parser(parser, args)
		
		self.formatting_options.add_argument("--x-grid", action='store_true', default=False,
		                                     help="Place an x-axes grid on the plot.")
		self.formatting_options.add_argument("--y-grid", action='store_true', default=False,
		                                     help="Place an y-axes grid on the plot.")
		self.formatting_options.add_argument("--z-grid", action='store_true', default=False,
		                                     help="Place an z-axes grid on the plot.")
		self.formatting_options.add_argument("-g", "--legloc", type=float, nargs=2, default=[0.6, 0.6],
		                                     help="Location (x/y coordinates) of the legend. [Default: %(default)s]")
		
	def prepare_args(self, parser, plotData):
		super(PlotRoot, self).prepare_args(parser, plotData)
		
		# defaults for colors
		for index, color in enumerate(plotData.plotdict["colors"]):
			if color == None:
				plotData.plotdict["colors"][index] = index + 1
			else:
				root_color = None
				if color.startswith("k"):
					root_color = getattr(ROOT, color)
				elif color.startswith("#"):
					root_color = ROOT.TColor.GetColor(color)
				else:
					root_color = eval(color)
				plotData.plotdict["colors"][index] = root_color
		self.set_default_ratio_colors(plotData)
		
		# defaults for markers
		for index, marker in enumerate(plotData.plotdict["markers"]):
			if marker == None:
				plotData.plotdict["markers"][index] = "E" if index == 0 else "HIST"
	
	def run(self, plotData):
		super(PlotRoot, self).run(plotData)

	def create_canvas(self, plotData):
		super(PlotRoot, self).create_canvas(plotData)
		
		if self.canvas == None:
			self.canvas = ROOT.TCanvas()
		
		if plotData.plotdict["ratio"]:
			self.plot_ratio_slider_y = 0.35
			self.canvas.cd()
			if self.plot_pad == None:
				self.plot_pad = ROOT.TPad("plot_pad", "", 0.0, self.plot_ratio_slider_y, 1.0, 1.0)
				self.plot_pad.SetNumber(1)
				self.plot_pad.Draw()
			if self.ratio_pad == None:
		   		self.ratio_pad = ROOT.TPad("ratio_pad", "", 0.0, 0.0, 1.0, self.plot_ratio_slider_y)
				self.ratio_pad.SetNumber(2)
				self.ratio_pad.Draw()
			
			self.plot_pad.SetBottomMargin(0.02)
			self.ratio_pad.SetBottomMargin(0.35);
		
		else:
			self.plot_pad = self.canvas
			

	def prepare_histograms(self, plotData):
		super(PlotRoot, self).prepare_histograms(plotData)
		
		for nick, color in zip(plotData.plotdict["nicks"], plotData.plotdict["colors"]):
			root_histogram = plotData.plotdict["root_objects"][nick]
			
			root_histogram.SetLineColor(color)
			root_histogram.SetFillColor(color)
			root_histogram.SetFillStyle(1)
			root_histogram.SetMarkerColor(color)
			
			# tick labels
			if plotData.plotdict["x_tick_labels"] and len(plotData.plotdict["x_tick_labels"]) > 0:
				for x_bin in range(min(root_histogram.GetNbinsX(), len(plotData.plotdict["x_tick_labels"]))):
					root_histogram.GetXaxis().SetBinLabel(x_bin+1, plotData.plotdict["x_tick_labels"][x_bin])
			
			if plotData.plotdict["y_tick_labels"] and len(plotData.plotdict["y_tick_labels"]) > 0:
				for y_bin in range(min(root_histogram.GetNbinsY(), len(plotData.plotdict["y_tick_labels"]))):
					root_histogram.GetYaxis().SetBinLabel(y_bin+1, plotData.plotdict["y_tick_labels"][y_bin])
			
			if plotData.plotdict["z_tick_labels"] and len(plotData.plotdict["z_tick_labels"]) > 0:
				for z_bin in range(min(root_histogram.GetNbinsZ(), len(plotData.plotdict["z_tick_labels"]))):
					root_histogram.GetZaxis().SetBinLabel(z_bin+1, plotData.plotdict["z_tick_labels"][z_bin])
	
	def make_plots(self, plotData):
		super(PlotRoot, self).make_plots(plotData)
	
		self.plot_pad.cd()
		
		self.x_min = sys.float_info.max
		self.x_max = -sys.float_info.max
		self.y_min = sys.float_info.max
		self.y_max = -sys.float_info.max
		self.z_min = sys.float_info.max
		self.z_max = -sys.float_info.max
		
		self.plot_sequence_indices = range(len(plotData.plotdict["nicks"]))
		self.plot_sequence_indices.sort(key=lambda index: "e" in plotData.plotdict["markers"][index].lower())
		for index, plot_index in enumerate(self.plot_sequence_indices):
			root_histogram = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][plot_index]]
			marker = plotData.plotdict["markers"][plot_index]
			option = marker + ("" if index == 0 else " same")
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
				option = "e" if index == 0 else "e same"
				root_histogram.Draw(option)
				if index == 0: self.first_plotted_ratio_histogram = root_histogram
			
				# trace min. and max. values of axes
				if root_histogram.GetMinimum() < self.ratio_y_min: self.ratio_y_min = root_histogram.GetMinimum()
				if root_histogram.GetMaximum() > self.ratio_y_max: self.ratio_y_max = root_histogram.GetMaximum()
			
	
	def modify_axes(self, plotData):
		super(PlotRoot, self).modify_axes(plotData)
	
		# axis labels
		self.first_plotted_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
		self.first_plotted_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
		self.first_plotted_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])
		if plotData.plotdict["ratio"]:
			self.first_plotted_ratio_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			self.first_plotted_ratio_histogram.GetYaxis().SetTitle(plotData.plotdict["y_ratio_label"])
	
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
			                                                     self.y_max * 1.1)
		else:
			self.first_plotted_histogram.GetYaxis().SetRangeUser(plotData.plotdict["y_lims"][0],
			                                                     plotData.plotdict["y_lims"][1])
		if plotData.plotdict["ratio"]:
			if plotData.plotdict["y_ratio_lims"] == None:
				self.first_plotted_ratio_histogram.GetYaxis().SetRangeUser(self.ratio_y_min,
					                                                       self.ratio_y_max * 1.1)
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
			self.first_plotted_histogram.GetYaxis().SetLabelSize(self.first_plotted_histogram.GetYaxis().GetLabelSize() / (1.0 - self.plot_ratio_slider_y))
			self.first_plotted_histogram.GetYaxis().SetTitleSize(self.first_plotted_histogram.GetYaxis().GetTitleSize() / (1.0 - self.plot_ratio_slider_y))
			
			self.first_plotted_histogram.GetYaxis().SetTitleOffset(self.first_plotted_histogram.GetYaxis().GetTitleOffset() * (1.0 - self.plot_ratio_slider_y))
			
			self.first_plotted_ratio_histogram.GetXaxis().SetLabelSize(self.first_plotted_ratio_histogram.GetXaxis().GetLabelSize() / self.plot_ratio_slider_y)
			self.first_plotted_ratio_histogram.GetXaxis().SetTitleSize(self.first_plotted_ratio_histogram.GetXaxis().GetTitleSize() / self.plot_ratio_slider_y)
			self.first_plotted_ratio_histogram.GetYaxis().SetLabelSize(self.first_plotted_ratio_histogram.GetYaxis().GetLabelSize() / self.plot_ratio_slider_y)
			self.first_plotted_ratio_histogram.GetYaxis().SetTitleSize(self.first_plotted_ratio_histogram.GetYaxis().GetTitleSize() / self.plot_ratio_slider_y)
			
			self.first_plotted_ratio_histogram.GetXaxis().SetTitleOffset(2.0 * self.first_plotted_ratio_histogram.GetXaxis().GetTitleOffset() * self.plot_ratio_slider_y)
			self.first_plotted_ratio_histogram.GetYaxis().SetTitleOffset(self.first_plotted_ratio_histogram.GetYaxis().GetTitleOffset() * self.plot_ratio_slider_y)
			
			self.first_plotted_ratio_histogram.GetYaxis().SetNdivisions(5, 0, 0)
			
		
	def add_labels(self, plotData):
		super(PlotRoot, self).add_labels(plotData)
		
		self.plot_pad.cd()
		self.legend = ROOT.TLegend(plotData.plotdict["legloc"][0], plotData.plotdict["legloc"][1], 0.9, 0.9);
		for plot_index in self.plot_sequence_indices[::-1]:
			root_histogram = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][plot_index]]
			marker = plotData.plotdict["markers"][plot_index]
			label = plotData.plotdict["labels"][plot_index]
			if label != None:
				self.legend.AddEntry(root_histogram, label, "LP" if "e" in marker.lower() else "F")
		self.legend.Draw()
	
	def add_texts(self, plotData):
		super(PlotRoot, self).add_texts(plotData)
	
	def save_canvas(self, plotData):
		super(PlotRoot, self).save_canvas(plotData)
		
		for output_filename in plotData.plotdict["output_filenames"]:
			self.canvas.SaveAs(output_filename)
			log.info("Created plot \"%s\"." % output_filename)

