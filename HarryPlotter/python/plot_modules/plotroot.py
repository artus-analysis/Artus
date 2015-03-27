# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy
import os
import ROOT
import sys

import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata
import Artus.HarryPlotter.utility.extrafunctions as extrafunctions

ROOT.PyConfig.IgnoreCommandLineOptions = True


class RootPlotContainer(plotdata.PlotContainer):
	def __init__(self, canvas=None, plot_pad=None, subplot_pad=None):
		self.canvas = canvas
		self.plot_pad = plot_pad
		self.subplot_pad = subplot_pad
	
	def finish(self):
		self.canvas.RedrawAxis()
		if not self.plot_pad is None:
			self.plot_pad.RedrawAxis()
			self.plot_pad.Update()
		if not self.subplot_pad is None:
			self.subplot_pad.RedrawAxis()
			self.subplot_pad.Update()
		self.canvas.Update()
		
	def save(self, filename):
		self.canvas.SaveAs(filename)


class PlotRoot(plotbase.PlotBase):
	def __init__(self):
		super(PlotRoot, self).__init__()
		
		self.text_boxes = []
		
		self.axes_histogram = None
		self.subplot_axes_histogram = None
	
	def modify_argument_parser(self, parser, args):
		super(PlotRoot, self).modify_argument_parser(parser, args)
		
		self.formatting_options.add_argument("-C", "--colors", type=str, nargs="+",
		                                     help="Colors for the plots. For each plot up to two colors (whitespace separated) can be specified, the first for lines and markers and the second for filled areas.")
		self.formatting_options.add_argument("--x-grid", nargs="?", type="bool", default=False, const=True,
		                                     help="Place an x-axes grid on the plot. [Default: %(default)s]")
		self.formatting_options.add_argument("--y-grid", nargs="?", type="bool", default=False, const=True,
		                                     help="Place an y-axes grid on the plot. [Default: %(default)s]")
		self.formatting_options.add_argument("--marker-styles", nargs="+", default=[20], type=int,
		                                     help="Marker style of plots marker. [Default: %(default)s]")
		self.formatting_options.add_argument("--marker-sizes", nargs="+", default=[1.0], type=float,
		                                     help="Marker sizes of plots marker. [Default: %(default)s]")
		self.formatting_options.add_argument("--fill-styles", type=int, nargs="+",
		                                     help="Fill styles for histograms. Defaults choosen according to draw options.")
		self.formatting_options.add_argument("--line-styles", nargs="+", default=[1], type=int,
		                                     help="Line style of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--line-widths", nargs="+", default=[2], type=int,
		                                     help="Line width of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--legend", type=float, nargs="*", default=None,
		                                     help="Legend position. The four arguments define the rectangle (x1 y1 x2 y2) for the legend. Without (or with too few) arguments, the default values from [0.6, 0.6, 0.9, 0.9] are used. [Default: %(default)s]")
		
	def prepare_args(self, parser, plotData):
		super(PlotRoot, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(
				plotData,
				["nicks", "colors", "labels", "markers", "marker_styles", "marker_sizes", "line_styles", "line_widths", "x_errors", "y_errors", "stacks", "fill_styles"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks", "stacks"] if plotData.plotdict[l] is not None]
		))
		
		# defaults for colors
		# per plot (up to) two colors are possible: first for lines and markers and second for filled areas
		for index, colors in enumerate(plotData.plotdict["colors"]):
			if colors == None:
				plotData.plotdict["colors"][index] = [index + 1, index + 1]
			else:
				colors = colors.split()
				if len(colors) == 1:
					colors = [colors[0], copy.deepcopy(colors[0])]
				
				for sub_index, color in enumerate(colors):
					if color.startswith("k"):
						color = getattr(ROOT, color)
					elif color.startswith("#"):
						color = ROOT.TColor.GetColor(color)
					else:
						color = eval(color)
					colors[sub_index] = color
				plotData.plotdict["colors"][index] = colors
		
		# defaults for markers
		for index, (line_width, marker, marker_style, fill_style, stack) in enumerate(zip(
				plotData.plotdict["line_widths"],
				plotData.plotdict["markers"],
				plotData.plotdict["marker_styles"],
				plotData.plotdict["fill_styles"],
				plotData.plotdict["stacks"]
		)):
			if marker is None:
				if index == 0:
					marker = "E" if len(plotData.plotdict["markers"]) > 1 else "HIST"
				else:
					marker = "LINE" if plotData.plotdict["stacks"].count(stack) == 1 else "HIST"
				# TODO: defaults for 2D/3D histograms
			
			if fill_style is None:
				fill_style = 0
				if "HIST" in marker.upper():
					line_width = 0
					fill_style = 1001
				elif "LINE" in marker.upper():
					marker = marker.upper().replace("LINE", "HIST")
					fill_style = 0
				elif "E" in marker.upper() and marker.upper() != "E":
					marker_style = 0
					fill_style = 3003
			
			plotData.plotdict["line_widths"][index] = line_width
			plotData.plotdict["markers"][index] = marker
			plotData.plotdict["marker_styles"][index] = marker_style
			plotData.plotdict["fill_styles"][index] = fill_style
		
		# defaults for legend position
		if not plotData.plotdict["legend"] is None:
			plotData.plotdict["legend"] += [0.6, 0.6, 0.9, 0.9][len(plotData.plotdict["legend"]):]
			plotData.plotdict["legend"] = plotData.plotdict["legend"][:4]
	
	def run(self, plotData):
		super(PlotRoot, self).run(plotData)

	def set_style(self, plotData):
		super(PlotRoot, self).set_style(plotData)
		
		# load TDR Style
		cwd = os.getcwd()
		os.chdir(os.path.expandvars("$CMSSW_BASE/src"))
		ROOT.gROOT.LoadMacro(os.path.expandvars("$CMSSW_BASE/src/Artus/HarryPlotter/python/utility/tdrstyle.C")) # +"+") # compilation currently does not work
		ROOT.setTDRStyle()
		os.chdir(cwd)

	def create_canvas(self, plotData):
		super(PlotRoot, self).create_canvas(plotData)
		
		canvas = None if plotData.plot is None else plotData.plot.canvas
		plot_pad = None if plotData.plot is None else plotData.plot.plot_pad
		subplot_pad = None if plotData.plot is None else plotData.plot.subplot_pad
		
		if canvas is None:
			# TODO: Creating the canvas like this leads to segmentation faults
			canvas = ROOT.TCanvas("canvas", "")
			canvas.Draw()

		#Modify right side margin for 2d-plots
		if not isinstance(plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]], ROOT.TGraph):
			if plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]].GetDimension() == 2:
				canvas.SetRightMargin(0.15)
		
		if len(plotData.plotdict["subplot_nicks"]) > 0:
			self.plot_subplot_slider_y = 0.35
			canvas.cd()
			if plot_pad is None:
				plot_pad = ROOT.TPad("plot_pad", "", 0.0, self.plot_subplot_slider_y, 1.0, 1.0)
				plot_pad.SetNumber(1)
				plot_pad.Draw()
			if subplot_pad is None:
				subplot_pad = ROOT.TPad("subplot_pad", "", 0.0, 0.0, 1.0, self.plot_subplot_slider_y)
				subplot_pad.SetNumber(2)
				subplot_pad.Draw()
			
			plot_pad.SetBottomMargin(0.02)
			subplot_pad.SetBottomMargin(0.35)
		
		else:
			plot_pad = canvas
		
		plotData.plot = RootPlotContainer(canvas, plot_pad, subplot_pad)

	def prepare_histograms(self, plotData):
		super(PlotRoot, self).prepare_histograms(plotData)
		
		for nick, colors, marker, marker_style, marker_size, fill_style, line_style, line_width in zip(
				plotData.plotdict["nicks"],
				plotData.plotdict["colors"],
				plotData.plotdict["markers"],
				plotData.plotdict["marker_styles"],
				plotData.plotdict["marker_sizes"],
				plotData.plotdict["fill_styles"],
				plotData.plotdict["line_styles"],
				plotData.plotdict["line_widths"]
		):
			root_object = plotData.plotdict["root_objects"][nick]
			if plotData.plotdict["title"]:
				root_object.SetTitle(plotData.plotdict["title"])
			
			root_object.SetLineColor(colors[0])
			root_object.SetLineStyle(line_style)
			root_object.SetLineWidth(line_width)
			
			root_object.SetMarkerColor(colors[0])
			root_object.SetMarkerStyle(marker_style)
			root_object.SetMarkerSize(marker_size)
			
			root_object.SetFillColor(colors[1])
			root_object.SetFillStyle(fill_style)

			# tick labels
			if plotData.plotdict["x_tick_labels"] and len(plotData.plotdict["x_tick_labels"]) > 0:
				for x_bin in range(min(root_object.GetNbinsX(), len(plotData.plotdict["x_tick_labels"]))):
					root_object.GetXaxis().SetBinLabel(x_bin+1, plotData.plotdict["x_tick_labels"][x_bin])
			
			if plotData.plotdict["y_tick_labels"] and len(plotData.plotdict["y_tick_labels"]) > 0:
				for y_bin in range(min(root_object.GetNbinsY(), len(plotData.plotdict["y_tick_labels"]))):
					root_object.GetYaxis().SetBinLabel(y_bin+1, plotData.plotdict["y_tick_labels"][y_bin])
			
			if plotData.plotdict["z_tick_labels"] and len(plotData.plotdict["z_tick_labels"]) > 0:
				for z_bin in range(min(root_object.GetNbinsZ(), len(plotData.plotdict["z_tick_labels"]))):
					root_object.GetZaxis().SetBinLabel(z_bin+1, plotData.plotdict["z_tick_labels"][z_bin])
	
	def make_plots(self, plotData):
		super(PlotRoot, self).make_plots(plotData)
		
		# draw empty histograms for the axes
		x_lims = plotData.plotdict["x_lims"] if not plotData.plotdict["x_lims"] is None else [self.x_min, self.x_max]
		y_lims = plotData.plotdict["y_lims"] if not plotData.plotdict["y_lims"] is None else [self.y_min * (0.5 if plotData.plotdict["y_log"] else 0.9), self.y_max * (2.0 if plotData.plotdict["y_log"] else 1.1)]
		z_lims = plotData.plotdict["z_lims"] if not plotData.plotdict["z_lims"] is None else [(self.z_min * (0.5 if plotData.plotdict["y_log"] else 0.9)) if self.z_min else self.z_min, (self.z_max * (2.0 if plotData.plotdict["z_log"] else 1.1)) if self.z_max else self.z_max]
		y_sub_lims = plotData.plotdict["y_subplot_lims"] if not plotData.plotdict["y_subplot_lims"] is None else [(self.y_sub_min * (1.1 if self.y_sub_min < 0.0 else 0.9)) if self.y_sub_min else self.y_sub_min, (self.y_sub_max * 1.1) if self.y_sub_max else self.y_sub_max]
		z_sub_lims = [self.z_sub_min, self.z_sub_max] # plotData.plotdict["z_subplot_lims"] if not plotData.plotdict["z_subplot_lims"] is None else [self.z_sub_min, self.z_sub_max]
		
		n_bins = 1 # TODO: consider axis ticks
		n_sub_bins = 1 # TODO: consider axis ticks
		
		if plotData.plot.plot_pad:
			plotData.plot.plot_pad.cd()
			if self.max_dim == 2:
				self.axes_histogram = ROOT.TH1F("axes_histogram", "", n_bins, x_lims[0], x_lims[1])
				self.axes_histogram.SetMinimum(y_lims[0])
				self.axes_histogram.SetMaximum(y_lims[1])
			else:
				self.axes_histogram = ROOT.TH2F("axes_histogram", "", n_bins, x_lims[0], x_lims[1], n_bins, y_lims[0], y_lims[1])
				self.axes_histogram.SetMinimum(z_lims[0])
				self.axes_histogram.SetMaximum(z_lims[1])
			self.axes_histogram.Draw("AXIS")
		
		if plotData.plot.subplot_pad:
			plotData.plot.subplot_pad.cd()
			if self.max_sub_dim == 2:
				self.subplot_axes_histogram = ROOT.TH1F("subplot_axes_histogram", "", n_sub_bins, x_lims[0], x_lims[1])
				self.subplot_axes_histogram.SetMinimum(y_sub_lims[0])
				self.subplot_axes_histogram.SetMaximum(y_sub_lims[1])
			else:
				self.subplot_axes_histogram = ROOT.TH2F("subplot_axes_histogram", "", n_sub_bins, x_lims[0], x_lims[1], n_sub_bins, y_sub_lims[0], y_sub_lims[1])
				self.subplot_axes_histogram.SetMinimum(z_sub_lims[0])
				self.subplot_axes_histogram.SetMaximum(z_sub_lims[1])
			self.subplot_axes_histogram.Draw("AXIS")
		
		for nick, subplot, marker in zip(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplots"],
				plotData.plotdict["markers"]
		):
			# select pad to plot on
			pad = plotData.plot.subplot_pad if subplot == True else plotData.plot.plot_pad
			pad.cd()
			
			# draw
			plotData.plotdict["root_objects"][nick].Draw(marker + " SAME")
	
	def modify_axes(self, plotData):
		super(PlotRoot, self).modify_axes(plotData)
	
		# axis labels
		self.axes_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
		self.axes_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
		self.axes_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])
		if not self.subplot_axes_histogram is None:
			self.subplot_axes_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			self.subplot_axes_histogram.GetYaxis().SetTitle(plotData.plotdict["y_subplot_label"])
	
		# logaritmic axis
		if plotData.plotdict["x_log"]: plotData.plot.plot_pad.SetLogx()
		if plotData.plotdict["y_log"]: plotData.plot.plot_pad.SetLogy()
		if plotData.plotdict["z_log"]: plotData.plot.plot_pad.SetLogz()
		if not plotData.plot.subplot_pad is None:
			if plotData.plotdict["x_log"]: plotData.plot.subplot_pad.SetLogx()
	
		# axis limits
		x_lims = plotData.plotdict["x_lims"] if not plotData.plotdict["x_lims"] is None else [self.x_min, self.x_max]
		y_lims = plotData.plotdict["y_lims"] if not plotData.plotdict["y_lims"] is None else [self.y_min, self.y_max]
		z_lims = plotData.plotdict["z_lims"] if not plotData.plotdict["z_lims"] is None else [self.z_min, self.z_max]
		#PlotRoot._set_axis_limits(self.axes_histogram, x_lims, y_lims, z_lims)
		
		if not self.subplot_axes_histogram is None:
			y_subplot_lims = plotData.plotdict["y_subplot_lims"] if not plotData.plotdict["y_subplot_lims"] is None else [self.y_sub_min, self.y_sub_max]
			z_subplot_lims = [self.z_sub_min, self.z_sub_max] # TODO: z-lims for possible 3D subplots
			#PlotRoot._set_axis_limits(self.subplot_axes_histogram, x_lims, y_subplot_lims, z_subplot_lims)
		
		if not self.subplot_axes_histogram is None:
			self.axes_histogram.GetXaxis().SetLabelSize(0)
			self.axes_histogram.GetXaxis().SetTitleSize(0)
			self.axes_histogram.GetYaxis().SetLabelSize(self.axes_histogram.GetYaxis().GetLabelSize() / (1.0 - self.plot_subplot_slider_y))
			self.axes_histogram.GetYaxis().SetTitleSize(self.axes_histogram.GetYaxis().GetTitleSize() / (1.0 - self.plot_subplot_slider_y))
			
			self.axes_histogram.GetYaxis().SetTitleOffset(self.axes_histogram.GetYaxis().GetTitleOffset() * (1.0 - self.plot_subplot_slider_y))
			
			self.subplot_axes_histogram.GetXaxis().SetLabelSize(self.subplot_axes_histogram.GetXaxis().GetLabelSize() / self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetXaxis().SetTitleSize(self.subplot_axes_histogram.GetXaxis().GetTitleSize() / self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetYaxis().SetLabelSize(self.subplot_axes_histogram.GetYaxis().GetLabelSize() / self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetYaxis().SetTitleSize(self.subplot_axes_histogram.GetYaxis().GetTitleSize() / self.plot_subplot_slider_y)
			
			self.subplot_axes_histogram.GetXaxis().SetTitleOffset(2.0 * self.subplot_axes_histogram.GetXaxis().GetTitleOffset() * self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetYaxis().SetTitleOffset(self.subplot_axes_histogram.GetYaxis().GetTitleOffset() * self.plot_subplot_slider_y)
			
			self.subplot_axes_histogram.GetYaxis().SetNdivisions(5, 0, 0)
		
		# redraw axes only and update the canvas
		plotData.plot.plot_pad.cd()
		self.axes_histogram.Draw("AXIS SAME")
		plotData.plot.plot_pad.Update()
		
		if not plotData.plot.subplot_pad is None:
			plotData.plot.subplot_pad.cd()
			if not self.subplot_axes_histogram is None:
				self.subplot_axes_histogram.Draw("AXIS SAME")
			plotData.plot.subplot_pad.Update()
		
		plotData.plot.canvas.Update()
			
		
	def add_grid(self, plotData):
		super(PlotRoot, self).add_grid(plotData)
		
		plotData.plot.plot_pad.cd()
		if (plotData.plotdict["grid"] or plotData.plotdict["x_grid"]):
			plotData.plot.plot_pad.SetGridx()
		if (plotData.plotdict["grid"] or plotData.plotdict["y_grid"]):
			plotData.plot.plot_pad.SetGridy()
		
		if not plotData.plot.subplot_pad is None:
			plotData.plot.subplot_pad.cd()
			if (plotData.plotdict["subplot_grid"] == True):
				plotData.plot.subplot_pad.SetGrid()
	
	def add_labels(self, plotData):
		super(PlotRoot, self).add_labels(plotData)
		
		plotData.plot.plot_pad.cd()
		self.legend = None
		if plotData.plotdict["legend"] != None:
			self.legend = ROOT.TLegend(*plotData.plotdict["legend"]);
			for nick, subplot, marker, fill_style, label in zip(
					plotData.plotdict["nicks"],
					plotData.plotdict["subplots"],
					plotData.plotdict["markers"],
					plotData.plotdict["fill_styles"],
					plotData.plotdict["labels"]
			):
				if subplot == True:
					# TODO handle possible subplot legends
					continue
				root_object = plotData.plotdict["root_objects"][nick]
				if (not label is None) and (label != ""):
					draw_option = "FLP"
					if isinstance(root_object, ROOT.TH1):
						draw_option = ("" if fill_style == 0 else "F") + ("EP" if "E" in marker.upper() else "") + "L"
					elif isinstance(root_object, ROOT.TGraph):
						draw_option = "ELP"
					elif isinstance(root_object, ROOT.TF1):
						draw_option = "L"
					self.legend.AddEntry(root_object, label, draw_option)
			self.legend.Draw()
	
	def add_texts(self, plotData):
		super(PlotRoot, self).add_texts(plotData)
		if plotData.plotdict["texts"] != [None]:
			plotData.plot.plot_pad.cd()
			for x, y, text in zip(plotData.plotdict['texts_x'], plotData.plotdict['texts_y'], plotData.plotdict['texts']):
				self.text_boxes.append(ROOT.TPaveText(0.0, 0.0, 1.0, 1.0, "NDC"))
				self.text_boxes[-1].AddText(x, y, text*5)
				self.text_boxes[-1].SetFillStyle(0)
				self.text_boxes[-1].SetShadowColor(0)
				self.text_boxes[-1].SetTextSize(0.05)
				self.text_boxes[-1].Draw("SAME")
	
	@staticmethod
	def _get_dimension(root_object):
		dimension = 2
		if isinstance(root_object, ROOT.TH1):
			dimension = root_object.GetDimension()
		elif isinstance(root_object, ROOT.TGraph2D):
			dimension = 3
		elif isinstance(root_object, ROOT.TGraph):
			dimension = 2
		else:
			log.warning("Retrieving the dimension of the plot is not yet implemented for objects of type %s!. Assume 2D plot." % str(type(axes_histogram)))
		return dimension
	
	@staticmethod
	def _set_axis_limits(root_object, x_lims=None, y_lims=None, z_lims=None):
		dimension = PlotRoot._get_dimension(root_object)
		
		if not x_lims is None:
			root_object.GetXaxis().SetRangeUser(*x_lims)
			root_object.GetXaxis().SetLimits(*x_lims)
		
		if (dimension > 1) and (not y_lims is None):
			root_object.GetYaxis().SetRangeUser(*y_lims)
			root_object.GetYaxis().SetLimits(*y_lims)
		
		if (dimension > 2) and (not z_lims is None):
			root_object.GetZaxis().SetRangeUser(*y_lims)
			root_object.GetZaxis().SetLimits(*y_lims)

