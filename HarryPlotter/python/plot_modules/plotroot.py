# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import collections
import copy
import math
import os
import ROOT
import sys
import re

import Artus.Utility.tools as tools

import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata
import Artus.HarryPlotter.utility.labels as labels
import Artus.HarryPlotter.utility.roottools as roottools

ROOT.PyConfig.IgnoreCommandLineOptions = True
import Artus.HarryPlotter.utility.defaultrootstyle as defaultrootstyle
import Artus.HarryPlotter.utility.tdrstyle as tdrstyle
import Artus.HarryPlotter.utility.CMS_lumi as CMS_lumi
import Artus.HarryPlotter.utility.newrootcolors as newrootcolors


class RootPlotContainer(plotdata.PlotContainer):
	def __init__(self, canvas=None, plot_pad=None, subplot_pad=None):
		self.canvas = canvas
		self.plot_pad = plot_pad
		self.subplot_pad = subplot_pad
	
	def finish(self):
		self.canvas.RedrawAxis("g")
		if not self.plot_pad is None:
			self.plot_pad.RedrawAxis("g")
			self.plot_pad.Update()
		if not self.subplot_pad is None:
			self.subplot_pad.RedrawAxis("g")
			self.subplot_pad.Update()
		self.canvas.Update()
		
	def save(self, filename):
		self.canvas.SaveAs(filename)


class PlotRoot(plotbase.PlotBase):
	def __init__(self):
		super(PlotRoot, self).__init__()
		
		self.plot_subplot_slider_y = 0.35
		
		self.text_boxes = []
		
		self.axes_histogram = None
		self.subplot_axes_histogram = None
		
		self.subplot_line_graphs = []
		self.plot_vertical_lines = []
		
		self.nice_labels = labels.LabelsDict(latex_version="root")
	
	def modify_argument_parser(self, parser, args):
		super(PlotRoot, self).modify_argument_parser(parser, args)
		
		self.axis_options.add_argument("--x-lims", type=float, nargs="+",
		                               help="Lower and Upper limit for x-axis.")
		self.axis_options.add_argument("--x-rel-lims", type=float, nargs=2, default=[1.0, 1.0],
		                               help="Relative lower and upper margin for auto x-lims. [Default: %(default)s]")
		self.axis_options.add_argument("--sym-x-lims", nargs="?", type="bool", default=False, const=True,
		                               help="Symmetric x-axis limits of the plot. The parameters of --x-lims are taken as <center> <range/2>")
		self.axis_options.add_argument("--y-lims", type=float, nargs="+",
		                               help="Lower and Upper limit for y-axis.")
		self.axis_options.add_argument("--sym-y-lims", nargs="?", type="bool", default=False, const=True,
		                               help="Symmetric y-axis limits of the plot. The parameters of --y-lims are taken as <center> <range/2>")
		self.axis_options.add_argument("--z-lims", type=float, nargs="+",
		                               help="Lower and Upper limit for z-axis.")
		self.axis_options.add_argument("--sym-z-lims", nargs="?", type="bool", default=False, const=True,
		                               help="Symmetric z-axis limits of the plot. The parameters of --z-lims are taken as <center> <range/2>")
		self.axis_options.add_argument("--y-subplot-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis of a possible subplot.")
		self.axis_options.add_argument("--y-subplot-log", nargs="?", type="bool", default=False, const=True,
		                               help="Logarithmic y axis for subplot..")
		self.axis_options.add_argument("--z-subplot-log", nargs="?", type="bool", default=False, const=True,
		                               help="Logarithmic z axis for subplot.")
		self.axis_options.add_argument("--sym-y-subplot-lims", nargs="?", type="bool", default=False, const=True,
		                               help="Symmetric y-axis limits of a possible subplot. The parameters of --y-subplot-lims are taken as <center> <range/2>")
		self.axis_options.add_argument("--reverse-x-axis", nargs="?", type="bool", default=False, const=True,
		                               help="Reverse X axis labelling. [Default: %(default)s]")
		self.axis_options.add_argument("--reverse-y-axis", nargs="?", type="bool", default=False, const=True,
		                               help="Reverse Y axis labelling. [Default: %(default)s]")
		self.axis_options.add_argument("--reverse-z-axis", nargs="?", type="bool", default=False, const=True,
		                               help="Reverse Z axis labelling. [Default: %(default)s]")
		
		self.canvas_options.add_argument("--left-pad-margin", type=float, default=None,
		                               help="Left margin of pad. [Default: automatically determined]")
		self.canvas_options.add_argument("--right-pad-margin", type=float, default=None,
		                               help="Right margin of pad. [Default: automatically determined]")
		self.canvas_options.add_argument("--top-pad-margin", type=float, default=None,
		                               help="Top margin of pad. [Default: automatically determined]")
		self.canvas_options.add_argument("--bottom-pad-margin", type=float, default=None,
		                               help="Bottom margin of pad. [Default: automatically determined]")
		
		self.formatting_options.add_argument("-C", "--colors", type=str, nargs="+",
		                                     help="Colors for the plots. For each plot up to two colors (whitespace separated) can be specified, the first for lines and markers and the second for filled areas.")
		self.formatting_options.add_argument("--colormap", nargs="?", type="bool", default=False, const=True,
		                                     help="Use colormap as defined by multiple colors (whitespace separated) in --colors). [Default: %(default)s]")
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
		self.formatting_options.add_argument("--legend-markers", type=str, nargs="+",
		                                     help="Draw options for legend entries.")
		self.formatting_options.add_argument("--extra-text", type=str, nargs="?", default = "",
		                                     help="Extra text written on plot, e.g. \"Preliminary\" ")
		self.formatting_options.add_argument("--cms", nargs="?", type="bool", default=False, const=True,
		                               help="Make a CMS publication plot (CMS inside frame). See https://ghm.web.cern.ch/ghm/plots/.")
		self.formatting_options.add_argument("--cms-outframe", nargs="?", type="bool", default=False, const=True,
		                               help="Make a CMS publication plot (CMS outside frame). See https://ghm.web.cern.ch/ghm/plots/.")
		self.formatting_options.add_argument("--year", type=str, nargs="?", default="",
											help="Year of data taking is written in front of integrated luminosity.")
		
	def prepare_args(self, parser, plotData):
		super(PlotRoot, self).prepare_args(parser, plotData)
		self.prepare_list_args(
				plotData,
				["nicks", "stacks", "colors", "labels", "markers", "line_styles", "line_widths", "marker_styles", "marker_sizes", "legend_markers", "fill_styles"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks", "stacks"] if plotData.plotdict[l] is not None]
		))
		
		# defaults for colors
		# per plot (up to) two colors are possible: first for lines and markers and second for filled areas
		for index, colors in enumerate(plotData.plotdict["colors"]):
			if colors == None:
				plotData.plotdict["colors"][index] = [index + 1, index + 1]
			else:
				colors = [self.predefined_colors.get_predefined_color(color) for color in colors.split()]
				if len(colors) == 1:
					colors = [colors[0], copy.deepcopy(colors[0])]
				
				for sub_index, color in enumerate(colors):
					if color.startswith("k"):
						color = eval("ROOT."+color)
					elif color.startswith("#"):
						color = ROOT.TColor.GetColor(color.upper())
					elif color in newrootcolors.newIdx.keys():
						color = newrootcolors.newIdx[color]
					else:
						color = eval(color)
					colors[sub_index] = color
				plotData.plotdict["colors"][index] = colors
		
		# defaults for markers
		for index, (line_width, marker, marker_style, fill_style, stack, legend_marker) in enumerate(zip(
				plotData.plotdict["line_widths"],
				plotData.plotdict["markers"],
				plotData.plotdict["marker_styles"],
				plotData.plotdict["fill_styles"],
				plotData.plotdict["stacks"],
				plotData.plotdict["legend_markers"],
		)):
			if marker is None:
				if index == 0:
					marker = "E" if len(plotData.plotdict["markers"]) > 1 else "HIST"
				else:
					marker = "LINE" if plotData.plotdict["stacks"].count(stack) == 1 else "HIST"
				
				if any([y_expression is not None for y_expression in plotData.plotdict.get("y_expressions", [])]):
					marker = "COLZ"
			
			if fill_style is None:
				fill_style = 0
				if ("HIST" in marker.upper()) and (not "E" in marker.upper()):
					line_width = 0
					fill_style = 1001
				elif "LINE" in marker.upper():
					marker = marker.upper().replace("LINE", "HIST")
					fill_style = 0
				elif ("E" in marker.upper()) and (not "HIST" in marker.upper()) and (marker.upper() != "E"):
					marker_style = 0
					fill_style = 3001
			
			if legend_marker is None:
				# TODO: implement defaults here
				#legend_marker = "FLP"
				pass
			
			plotData.plotdict["line_widths"][index] = line_width
			plotData.plotdict["markers"][index] = marker
			plotData.plotdict["marker_styles"][index] = marker_style
			plotData.plotdict["fill_styles"][index] = fill_style
			plotData.plotdict["legend_markers"][index] = legend_marker
		
		# defaults for legend position
		if not plotData.plotdict["legend"] is None:
			plotData.plotdict["legend"] += [0.6, 0.6, 0.9, 0.9][len(plotData.plotdict["legend"]):]
			plotData.plotdict["legend"] = plotData.plotdict["legend"][:4]
		
		for key in ["labels", "x_label", "y_label", "x_tick_labels", "y_tick_labels", "z_label", "title"]:
			if isinstance(plotData.plotdict[key], basestring):
				plotData.plotdict[key] = self.nice_labels.get_nice_label(plotData.plotdict[key])
			elif isinstance(plotData.plotdict[key], collections.Iterable):
				plotData.plotdict[key] = [self.nice_labels.get_nice_label(label) for label in plotData.plotdict[key]]
	
	def run(self, plotData):
		super(PlotRoot, self).run(plotData)

	def set_style(self, plotData):
		super(PlotRoot, self).set_style(plotData)
		
		tdrstyle.setTDRStyle()
		defaultrootstyle.set_default_root_style()
		
		# load custom painter (fixes for horizontal histograms)
		roottools.RootTools.load_compile_macro(os.path.expandvars("$ARTUSPATH/HarryPlotter/python/utility/customhistogrampainter.C"))

	def create_canvas(self, plotData):
		super(PlotRoot, self).create_canvas(plotData)
		
		canvas = None if plotData.plot is None else plotData.plot.canvas
		plot_pad = None if plotData.plot is None else plotData.plot.plot_pad
		subplot_pad = None if plotData.plot is None else plotData.plot.subplot_pad
		
		if canvas is None:
			# TODO: Creating the canvas like this leads to segmentation faults
			canvas = defaultrootstyle.make_canvas("canvas", "", dx=plotData.plotdict["canvas_width"], dy=plotData.plotdict["canvas_height"])
			canvas.Draw()
		
		if len(plotData.plotdict["subplot_nicks"]) > 0:
			if subplot_pad is None:
				canvas.cd()
				plot_pad = ROOT.TPad("plot_pad", "", 0.0, self.plot_subplot_slider_y, 1.0, 1.0)
				plot_pad.SetNumber(1)
				plot_pad.Draw()
				defaultrootstyle.init_sub_pad(plot_pad)
			if subplot_pad is None:
				canvas.cd()
				subplot_pad = ROOT.TPad("subplot_pad", "", 0.0, 0.0, 1.0, self.plot_subplot_slider_y)
				subplot_pad.SetNumber(2)
				subplot_pad.Draw()
				defaultrootstyle.init_sub_pad(subplot_pad)
			
			plot_pad.SetTopMargin(0.122)
			plot_pad.SetBottomMargin(0.031)
			subplot_pad.SetBottomMargin(0.35)
			
			for index, y_line in enumerate(plotData.plotdict["subplot_lines"]):
				line_graph = ROOT.TGraph(2)
				line_graph.SetName("line_graph_"+str(index)+"_"+str(y_line))
				line_graph.SetTitle()
				
				line_graph.SetPoint(0, -sys.float_info.max, y_line)
				line_graph.SetPoint(1, +sys.float_info.max, y_line)
				
				line_graph.SetLineColor(ROOT.TColor.GetColor("#808080"))
				line_graph.SetLineWidth(3)
				line_graph.SetLineStyle(2)
				self.subplot_line_graphs.append(line_graph)
		else:
			plot_pad = canvas
			plot_pad.Draw()
		for index, x_line in enumerate(plotData.plotdict["vertical_lines"]):
			line_graph = ROOT.TGraph(2)
			line_graph.SetName("vertical_line_graph_"+str(index)+"_"+str(x_line))
			line_graph.SetTitle()

			line_graph.SetPoint(0, x_line, -sys.float_info.max)
			line_graph.SetPoint(1, x_line, +sys.float_info.max)

			line_graph.SetLineColor(2)
			line_graph.SetLineWidth(3)
			line_graph.SetLineStyle(2)
			self.plot_vertical_lines.append(line_graph)
		
		self.plot_pad_right_margin = plot_pad.GetRightMargin()
		plot_pad.SetRightMargin(0.25)
		if not subplot_pad is None:
			subplot_pad.SetRightMargin(0.25)
		if not plotData.plotdict["left_pad_margin"] is None:
			plot_pad.SetLeftMargin(plotData.plotdict["left_pad_margin"])
			if not subplot_pad is None:
				subplot_pad.SetLeftMargin(plotData.plotdict["left_pad_margin"])
		if not plotData.plotdict["right_pad_margin"] is None:
			plot_pad.SetRightMargin(plotData.plotdict["right_pad_margin"])
			if not subplot_pad is None:
				subplot_pad.SetRightMargin(plotData.plotdict["right_pad_margin"])
		if not plotData.plotdict["top_pad_margin"] is None:
			plot_pad.SetTopMargin(plotData.plotdict["top_pad_margin"])
		if not plotData.plotdict["bottom_pad_margin"] is None:
			if subplot_pad is None:
				plot_pad.SetBottomMargin(plotData.plotdict["bottom_pad_margin"])
			else:
				subplot_pad.SetBottomMargin(plotData.plotdict["bottom_pad_margin"])
		
		plotData.plot = RootPlotContainer(canvas, plot_pad, subplot_pad)

	def prepare_histograms(self, plotData):
		super(PlotRoot, self).prepare_histograms(plotData)
		
		first_plot_labels_set = False
		first_subplot_labels_set = False
		for nick, subplot, colors, marker, marker_style, marker_size, fill_style, line_style, line_width in zip(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplots"],
				plotData.plotdict["colors"],
				plotData.plotdict["markers"],
				plotData.plotdict["marker_styles"],
				plotData.plotdict["marker_sizes"],
				plotData.plotdict["fill_styles"],
				plotData.plotdict["line_styles"],
				plotData.plotdict["line_widths"]
		):
			root_object = plotData.plotdict["root_objects"][nick]
			
			root_object.SetLineColor(colors[0])
			root_object.SetLineStyle(line_style)
			root_object.SetLineWidth(line_width)
			
			root_object.SetMarkerColor(colors[0])
			root_object.SetMarkerStyle(marker_style)
			root_object.SetMarkerSize(marker_size)
			
			root_object.SetFillColor(colors[1])
			root_object.SetFillStyle(fill_style)
			
			# axis labels
			if subplot:
				if (not first_subplot_labels_set) or (not isinstance(root_object, ROOT.TF1)):
					if (not plotData.plotdict["x_label"] is None) and (plotData.plotdict["x_label"] != ""):
						root_object.GetXaxis().SetTitle(plotData.plotdict["x_label"])
					if (not plotData.plotdict["y_subplot_label"] is None) and (plotData.plotdict["y_subplot_label"] != ""):
						root_object.GetYaxis().SetTitle(plotData.plotdict["y_subplot_label"])
					#if (not plotData.plotdict["z_subplot_label"] is None) and (plotData.plotdict["z_subplot_label"] != ""):
					#	root_object.GetZaxis().SetTitle(plotData.plotdict["z_subplot_label"])
				first_subplot_labels_set = True
			else:
				if (not first_plot_labels_set) or (not isinstance(root_object, ROOT.TF1)):
					if (not plotData.plotdict["x_label"] is None) and (plotData.plotdict["x_label"] != ""):
						root_object.GetXaxis().SetTitle(plotData.plotdict["x_label"])
					if (not plotData.plotdict["y_label"] is None) and (plotData.plotdict["y_label"] != ""):
						root_object.GetYaxis().SetTitle(plotData.plotdict["y_label"])
					if (not plotData.plotdict["z_label"] is None) and (plotData.plotdict["z_label"] != "" and hasattr(root_object, "GetZaxis")):
						root_object.GetZaxis().SetTitle(plotData.plotdict["z_label"])
				first_plot_labels_set = True
			
			# tick labels (z-axis) TODO: unify this with the x and y bin labels mechanism?
			if plotData.plotdict["z_tick_labels"] and len(plotData.plotdict["z_tick_labels"]) > 0:
				for z_bin in range(min(root_object.GetNbinsZ(), len(plotData.plotdict["z_tick_labels"]))):
					root_object.GetZaxis().SetBinLabel(z_bin+1, plotData.plotdict["z_tick_labels"][z_bin])
			
			ROOT.TGaxis.SetMaxDigits(3)

	def determine_plot_lims(self, plotData):
		super(PlotRoot, self).determine_plot_lims(plotData)
		
		# x lims
		if plotData.plotdict["sym_x_lims"] and (not plotData.plotdict["x_log"]):
			if not plotData.plotdict["x_lims"] is None and len(plotData.plotdict["x_lims"]) > 1:
				self.x_min = plotData.plotdict["x_lims"][0] - plotData.plotdict["x_lims"][1]
				self.x_max = plotData.plotdict["x_lims"][0] + plotData.plotdict["x_lims"][1]
			else:
				tmp_x_min = self.x_min * (plotData.plotdict["x_rel_lims"][0] if self.x_min > 0.0 else 2.0-plotData.plotdict["x_rel_lims"][0])
				tmp_x_max = self.x_max * (plotData.plotdict["x_rel_lims"][1] if self.x_max > 0.0 else 2.0-plotData.plotdict["x_rel_lims"][1])
				
				if not plotData.plotdict["x_lims"] is None:
					center = plotData.plotdict["x_lims"][0]
					width = max([abs(x - center) for x in [tmp_x_min, tmp_x_max]])
					self.x_min = center - width
					self.x_max = center + width
				else:
					self.x_min = tmp_x_min
					self.x_max = tmp_x_max
		else:
			if plotData.plotdict["sym_x_lims"]:
				log.warning("Symmetric limits are not yet implemented for logarithmic axes!")
			
			if not plotData.plotdict["x_lims"] is None:
				self.x_min = plotData.plotdict["x_lims"][0]
				if len(plotData.plotdict["x_lims"]) > 1:
					self.x_max = plotData.plotdict["x_lims"][1]
				else:
					self.x_max *= (plotData.plotdict["x_rel_lims"][1] if self.x_max > 0.0 else 2.0-plotData.plotdict["x_rel_lims"][1])
			else:
				self.x_min *= (plotData.plotdict["x_rel_lims"][0] if self.x_min > 0.0 else 2.0-plotData.plotdict["x_rel_lims"][0])
				self.x_max *= (plotData.plotdict["x_rel_lims"][1] if self.x_max > 0.0 else 2.0-plotData.plotdict["x_rel_lims"][1])
		if self.x_min == self.x_max:
			self.x_min -= 1.0
			self.x_max += 1.0
		
		# y lims
		if plotData.plotdict["sym_y_lims"] and (not plotData.plotdict["y_log"]):
			if not plotData.plotdict["y_lims"] is None and len(plotData.plotdict["y_lims"]) > 1:
				self.y_min = plotData.plotdict["y_lims"][0] - plotData.plotdict["y_lims"][1]
				self.y_max = plotData.plotdict["y_lims"][0] + plotData.plotdict["y_lims"][1]
			else:
				tmp_y_min = self.y_min * ((plotData.plotdict["y_rel_lims"][0] if self.y_min > 0.0 else 2.0-plotData.plotdict["y_rel_lims"][0]) if self.max_dim < 3 else 1.0)
				tmp_y_max = self.y_max * ((plotData.plotdict["y_rel_lims"][1] if self.y_max > 0.0 else 2.0-plotData.plotdict["y_rel_lims"][1]) if self.max_dim < 3 else 1.0)
				if not plotData.plotdict["y_lims"] is None:
					center = plotData.plotdict["y_lims"][0]
					width = max([abs(y - center) for y in [tmp_y_min, tmp_y_max]])
					self.y_min = center - width
					self.y_max = center + width
				else:
					self.y_min = tmp_y_min
					self.y_max = tmp_y_max
		else:
			if plotData.plotdict["sym_y_lims"]:
				log.warning("Symmetric limits are not yet implemented for logarithmic axes!")
			
			if not plotData.plotdict["y_lims"] is None:
				self.y_min = plotData.plotdict["y_lims"][0]
			elif self.max_dim < 3:
				self.y_min *= (plotData.plotdict["y_rel_lims"][0] if self.y_min > 0.0 else 2.0-plotData.plotdict["y_rel_lims"][0])
		
			if not plotData.plotdict["y_lims"] is None and len(plotData.plotdict["y_lims"]) > 1:
				self.y_max = plotData.plotdict["y_lims"][1]
			elif self.max_dim < 3:
				self.y_max *= (plotData.plotdict["y_rel_lims"][1] if self.y_max > 0.0 else 2.0-plotData.plotdict["y_rel_lims"][1])
				if plotData.plotdict["cms"]:
					self.y_max *= 1.2

		if self.y_min == self.y_max:
			self.y_max += 1.0
		
		# z lims
		if plotData.plotdict["sym_z_lims"] and (not plotData.plotdict["z_log"]):
			if not plotData.plotdict["z_lims"] is None and len(plotData.plotdict["z_lims"]) > 1:
				self.z_min = plotData.plotdict["z_lims"][0] - plotData.plotdict["z_lims"][1]
				self.z_max = plotData.plotdict["z_lims"][0] + plotData.plotdict["z_lims"][1]
			else:
				tmp_z_min = self.z_min * (0.99 if self.z_min > 0.0 else 1.01)
				tmp_z_max = self.z_max * (1.01 if self.z_max > 0.0 else 0.99)
				if not plotData.plotdict["z_lims"] is None:
					center = plotData.plotdict["z_lims"][0]
					width = max([abs(z - center) for z in [tmp_z_min, tmp_z_max]])
					self.z_min = center - width
					self.z_max = center + width
				else:
					self.z_min = tmp_z_min
					self.z_max = tmp_z_max
		else:
			if plotData.plotdict["sym_z_lims"]:
				log.warning("Symmetric limits are not yet implemented for logarithmic axes!")
			
			if not plotData.plotdict["z_lims"] is None:
				self.z_min = plotData.plotdict["z_lims"][0]
			elif not self.z_min is None:
				if plotData.plotdict["z_log"]:
					self.z_min *= (0.9 if self.z_min > 0.0 else 1.1)
				else:
					self.z_min *= (0.99 if self.z_min > 0.0 else 1.01)
			
			if not plotData.plotdict["z_lims"] is None and len(plotData.plotdict["z_lims"]) > 1:
				self.z_max = plotData.plotdict["z_lims"][1]
			elif not self.z_max is None:
				if plotData.plotdict["z_log"]:
					self.z_max *= (1.1 if self.z_max > 0.0 else 0.9)
				else:
					self.z_max *= (1.01 if self.z_max > 0.0 else 0.99)
		if (not self.z_min is None) and (not self.z_max is None) and (self.z_min == self.z_max):
			self.z_max += 1.0
		
		# y subplot lims
		if plotData.plotdict["sym_y_subplot_lims"]:
			if not plotData.plotdict["y_subplot_lims"] is None and len(plotData.plotdict["y_subplot_lims"]) > 1:
				self.y_sub_min = plotData.plotdict["y_subplot_lims"][0] - plotData.plotdict["y_subplot_lims"][1]
				self.y_sub_max = plotData.plotdict["y_subplot_lims"][0] + plotData.plotdict["y_subplot_lims"][1]
			else:
				tmp_y_sub_min = self.y_sub_min * (0.9 if self.y_sub_min > 0.0 else 1.1)
				tmp_y_sub_max = self.y_sub_max * (1.1 if self.y_sub_max > 0.0 else 0.9)
				if not plotData.plotdict["y_subplot_lims"] is None:
					center = plotData.plotdict["y_subplot_lims"][0]
					width = max([abs(y - center) for y in [tmp_y_sub_min, tmp_y_sub_max]])
					self.y_sub_min = center - width
					self.y_sub_max = center + width
				else:
					self.y_sub_min = tmp_y_sub_min
					self.y_sub_max = tmp_y_sub_max
		else:
			if not plotData.plotdict["y_subplot_lims"] is None:
				self.y_sub_min = plotData.plotdict["y_subplot_lims"][0]
			elif not self.y_sub_min is None:
				self.y_sub_min *= (0.9 if self.y_sub_min > 0.0 else 1.1)
		
			if not plotData.plotdict["y_subplot_lims"] is None and len(plotData.plotdict["y_subplot_lims"]) > 1:
				self.y_sub_max = plotData.plotdict["y_subplot_lims"][1]
			elif not self.y_sub_max is None:
				self.y_sub_max *= (1.1 if self.y_sub_max > 0.0 else 0.9)
		if (not self.y_sub_min is None) and (not self.y_sub_max is None) and (self.y_sub_min == self.y_sub_max):
			self.y_sub_max += 1.0
		
		# z subplot lims
		if not self.z_sub_min is None:
			self.z_sub_min *= (0.9 if self.z_sub_min > 0.0 else 1.1)
		if not self.z_sub_max is None:
			self.z_sub_max *= (1.1 if self.z_sub_max > 0.0 else 0.9)
		if (not self.z_sub_min is None) and (not self.z_sub_max is None) and (self.z_sub_min == self.z_sub_max):
			self.z_sub_max += 1.0
		
	def make_plots(self, plotData):
		super(PlotRoot, self).make_plots(plotData)
		
		# draw empty histograms for the axes
		n_binsX = len(plotData.plotdict["x_tick_labels"]) if plotData.plotdict["x_tick_labels"] else 1
		n_binsY = len(plotData.plotdict["y_tick_labels"]) if plotData.plotdict["y_tick_labels"] else 1
		n_sub_binsX = len(plotData.plotdict["x_tick_labels"]) if plotData.plotdict["x_tick_labels"] else 1
		n_sub_binsY = len(plotData.plotdict["y_tick_labels"]) if plotData.plotdict["y_tick_labels"] else 1
		
		if plotData.plot.plot_pad:
			plotData.plot.plot_pad.cd()
			if self.max_dim == 2:
				self.axes_histogram = ROOT.TH2F("axes_histogram", "", n_binsX, self.x_min, self.x_max, n_binsY, self.y_min, self.y_max)
				self.axes_histogram.SetMinimum(self.y_min)
				self.axes_histogram.SetMaximum(self.y_max)
			else:
				self.axes_histogram = ROOT.TH2F("axes_histogram", "", n_binsX, self.x_min, self.x_max, n_binsY, self.y_min, self.y_max)
				self.axes_histogram.SetMinimum(self.z_min)
				self.axes_histogram.SetMaximum(self.z_max)
			
			# axis labels
			if (not plotData.plotdict["x_label"] is None) and (plotData.plotdict["x_label"] != ""):
				self.axes_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			if (not plotData.plotdict["y_label"] is None) and (plotData.plotdict["y_label"] != ""):
				self.axes_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
			if (self.max_dim > 2) and (not plotData.plotdict["z_label"] is None) and (plotData.plotdict["z_label"] != ""):
				self.axes_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])

			# tick labels
			if plotData.plotdict["x_tick_labels"] and len(plotData.plotdict["x_tick_labels"]) > 0:
				for x_bin in range(n_binsX):
					self.axes_histogram.GetXaxis().SetBinLabel(x_bin+1, plotData.plotdict["x_tick_labels"][x_bin])
			if plotData.plotdict["y_tick_labels"] and len(plotData.plotdict["y_tick_labels"]) > 0:
				for y_bin in range(n_binsY):
					self.axes_histogram.GetYaxis().SetBinLabel(y_bin+1, plotData.plotdict["y_tick_labels"][y_bin])
			
			# avoid scientific notation for x-axis
			self.axes_histogram.GetXaxis().SetNoExponent(True)
			
			# shift the exponent for y-axis to avoid overlapping with title
			ROOT.TGaxis.SetExponentOffset(-0.069,0.015,"y")
			
			self.axes_histogram.Draw("AXIS")

			for line_graph in self.plot_vertical_lines:
				line_graph.Draw("L SAME")
		
		if plotData.plot.subplot_pad:
			plotData.plot.subplot_pad.cd()
			if self.max_sub_dim == 2:
				self.subplot_axes_histogram = ROOT.TH2F("subplot_axes_histogram", "", n_sub_binsX, self.x_min, self.x_max, n_sub_binsY, self.y_sub_min, self.y_sub_max)
				self.subplot_axes_histogram.SetMinimum(self.y_sub_min)
				self.subplot_axes_histogram.SetMaximum(self.y_sub_max)
			else:
				self.subplot_axes_histogram = ROOT.TH2F("subplot_axes_histogram", "", n_sub_bins, self.x_min, self.x_max, n_sub_bins, self.y_sub_min, self.y_sub_max)
				self.subplot_axes_histogram.SetMinimum(self.z_sub_min)
				self.subplot_axes_histogram.SetMaximum(self.z_sub_max)
			
			# axis labels
			if (not plotData.plotdict["x_label"] is None) and (plotData.plotdict["x_label"] != ""):
				self.subplot_axes_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			if (not plotData.plotdict["y_subplot_label"] is None) and (plotData.plotdict["y_subplot_label"] != ""):
				self.subplot_axes_histogram.GetYaxis().SetTitle(plotData.plotdict["y_subplot_label"])
			#if (self.max_sub_dim > 2) and (not plotData.plotdict["z_subplot_label"] is None) and (plotData.plotdict["z_subplot_label"] != ""):
			#	self.subplot_axes_histogram.GetZaxis().SetTitle(plotData.plotdict["z_subplot_label"])
			
			# tick labels
			if plotData.plotdict["x_tick_labels"] and len(plotData.plotdict["x_tick_labels"]) > 0:
				for x_bin in range(n_sub_binsX):
					self.subplot_axes_histogram.GetXaxis().SetBinLabel(x_bin+1, plotData.plotdict["x_tick_labels"][x_bin])
			
			# avoid scientific notation for x-axis
			self.subplot_axes_histogram.GetXaxis().SetNoExponent(True)
			
			self.subplot_axes_histogram.Draw("AXIS")
			
			for line_graph in self.subplot_line_graphs:
				line_graph.Draw("L SAME")
		
		for nick, subplot, marker, colors in zip(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplots"],
				plotData.plotdict["markers"],
				plotData.plotdict["colors"]
		):
			# select pad to plot on
			pad = plotData.plot.subplot_pad if subplot else plotData.plot.plot_pad
			pad.cd()
			
			# set color map
			if plotData.plotdict["colormap"]:
				if len(set(colors)) == 1:
					ROOT.gStyle.SetPalette(colors[0])
				else:
					reds = [ROOT.gROOT.GetColor(color).GetRed() for color in colors]
					greens = [ROOT.gROOT.GetColor(color).GetGreen() for color in colors]
					blues = [ROOT.gROOT.GetColor(color).GetBlue() for color in colors]
					ROOT.TColor.CreateGradientColorTable(
							len(colors),
							array.array("d", [float(index) / (len(colors)-1) for index in xrange(len(colors))]),
							array.array("d", reds),
							array.array("d", greens),
							array.array("d", blues),
							ROOT.gStyle.GetNdivisions("Z")
					)
			
			# draw
			root_object = plotData.plotdict["root_objects"][nick]
			if "RTOL" in marker.upper() and isinstance(root_object, ROOT.TH1):
				root_object.__class__ = ROOT.CustomHistogram
				root_object.GetPainter()
			
			root_object.Draw(marker + " SAME")
			pad.Update()
	
	def modify_axes(self, plotData):
		super(PlotRoot, self).modify_axes(plotData)
		
		# setting for Z axis
		for root_object in plotData.plotdict["root_objects"].values():
			if isinstance(root_object, ROOT.TH1):
				palette = root_object.GetListOfFunctions().FindObject("palette")
			else:
				palette = root_object.GetHistogram().GetListOfFunctions().FindObject("palette")
			
			if palette != None:
				root_object.GetZaxis().SetTitleOffset(1.5)
				palette.SetTitleOffset(1.5)
				palette.SetTitleSize(root_object.GetYaxis().GetTitleSize())
				root_object.SetContour(50) # number of divisions
		
		# logaritmic axis
		if plotData.plotdict["x_log"]:
			plotData.plot.plot_pad.SetLogx()
			self.axes_histogram.GetXaxis().SetMoreLogLabels((math.log10(self.x_max) - math.log10(self.x_min)) < 3.0)
		if plotData.plotdict["y_log"]:
			plotData.plot.plot_pad.SetLogy()
			self.axes_histogram.GetYaxis().SetMoreLogLabels((math.log10(self.y_max) - math.log10(self.y_min)) < 3.0)
		if plotData.plotdict["z_log"]:
			plotData.plot.plot_pad.SetLogz()
			self.axes_histogram.GetZaxis().SetMoreLogLabels((math.log10(self.z_max) - math.log10(self.z_min)) < 3.0)
		
		if not plotData.plot.subplot_pad is None:
			if plotData.plotdict["x_log"]:
				plotData.plot.subplot_pad.SetLogx()
				self.subplot_axes_histogram.GetXaxis().SetMoreLogLabels((math.log10(self.x_max) - math.log10(self.x_min)) < 3.0)
			if plotData.plotdict["y_subplot_log"]:
				plotData.plot.subplot_pad.SetLogy()
				self.subplot_axes_histogram.GetYaxis().SetMoreLogLabels((math.log10(self.y_sub_max) - math.log10(self.y_sub_min)) < 3.0)
			if plotData.plotdict["z_subplot_log"]:
				plotData.plot.subplot_pad.SetLogz()
				self.subplot_axes_histogram.GetZaxis().SetMoreLogLabels((math.log10(self.z_sub_max) - math.log10(self.z_sub_min)) < 3.0)
		
		if not self.axes_histogram is None:
			self.reversed_axes = PlotRoot._set_axis_limits(plotData.plot.plot_pad, self.axes_histogram, self.max_dim, [self.x_min, self.x_max], [self.y_min, self.y_max], [self.z_min, self.z_max], reverse_x_axis=plotData.plotdict["reverse_x_axis"], reverse_y_axis=plotData.plotdict["reverse_y_axis"], reverse_z_axis=plotData.plotdict["reverse_z_axis"])
		if not self.subplot_axes_histogram is None:
			self.reversed_subplot_axes = PlotRoot._set_axis_limits(plotData.plot.subplot_pad, self.subplot_axes_histogram, self.max_dim, [self.x_min, self.x_max], [self.y_sub_min, self.y_sub_max], [self.z_sub_min, self.z_sub_max], reverse_x_axis=plotData.plotdict["reverse_x_axis"], reverse_y_axis=False, reverse_z_axis=False)
		
		for nick, subplot, marker in zip(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplots"],
				plotData.plotdict["markers"]
		):
			root_object = plotData.plotdict["root_objects"][nick]
			if subplot:
				PlotRoot._set_axis_limits(plotData.plot.subplot_pad, root_object, self.max_dim, [self.x_min, self.x_max], [self.y_sub_min, self.y_sub_max], [self.z_sub_min, self.z_sub_max], reverse_x_axis=plotData.plotdict["reverse_x_axis"], reverse_y_axis=False, reverse_z_axis=False)
			else:
				PlotRoot._set_axis_limits(plotData.plot.plot_pad, root_object, self.max_dim, [self.x_min, self.x_max], [self.y_min, self.y_max], [self.z_min, self.z_max], reverse_x_axis=plotData.plotdict["reverse_x_axis"], reverse_y_axis=plotData.plotdict["reverse_y_axis"], reverse_z_axis=plotData.plotdict["reverse_z_axis"])
		
		if not self.subplot_axes_histogram is None:
			self.axes_histogram.GetXaxis().SetLabelSize(0)
			self.axes_histogram.GetXaxis().SetTitleSize(0)
			self.axes_histogram.GetYaxis().SetLabelSize(self.axes_histogram.GetYaxis().GetLabelSize() / (1.0 - self.plot_subplot_slider_y))
			self.axes_histogram.GetYaxis().SetTitleSize((self.axes_histogram.GetYaxis().GetTitleSize() / (1.0 - self.plot_subplot_slider_y))-0.01)

			self.axes_histogram.GetYaxis().SetTitleOffset(self.axes_histogram.GetYaxis().GetTitleOffset() * (1.0 - self.plot_subplot_slider_y))
			
			self.subplot_axes_histogram.GetXaxis().SetLabelSize(self.subplot_axes_histogram.GetXaxis().GetLabelSize() / self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetXaxis().SetTitleSize((self.subplot_axes_histogram.GetXaxis().GetTitleSize() / self.plot_subplot_slider_y) - 0.01)
			self.subplot_axes_histogram.GetYaxis().SetLabelSize(self.subplot_axes_histogram.GetYaxis().GetLabelSize() / self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetYaxis().SetTitleSize((self.subplot_axes_histogram.GetYaxis().GetTitleSize() / self.plot_subplot_slider_y) - 0.01)
			
			self.subplot_axes_histogram.GetXaxis().SetTitleOffset(2.0 * self.subplot_axes_histogram.GetXaxis().GetTitleOffset() * self.plot_subplot_slider_y+0.2)
			self.subplot_axes_histogram.GetYaxis().SetTitleOffset(self.subplot_axes_histogram.GetYaxis().GetTitleOffset() * self.plot_subplot_slider_y)
			self.subplot_axes_histogram.GetYaxis().SetNdivisions(5, 0, 0)
		
		palettes = [(root_object if isinstance(root_object, ROOT.TH1) else root_object.GetHistogram()).GetListOfFunctions().FindObject("palette") for root_object in plotData.plotdict["root_objects"].values()]
		if all([palette == None for palette in palettes]) and (plotData.plotdict["right_pad_margin"] is None):
			plotData.plot.plot_pad.SetRightMargin(0.05)
			if not plotData.plot.subplot_pad is None:
				plotData.plot.subplot_pad.SetRightMargin(0.05)

		if (self.max_dim < 3) and (plotData.plotdict["right_pad_margin"] is None):
			plotData.plot.plot_pad.SetRightMargin(0.05)
			if not plotData.plot.subplot_pad is None:
				plotData.plot.subplot_pad.SetRightMargin(0.05)
		
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
		#tdrstyle.fixOverlay(self.plot_pad)
		
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
		
		# TODO: transform legend coordinates so that same values for plots with subplots can be specified
		"""
		pad_pos_x_pixel = [plotData.plot.plot_pad.UtoPixel(x) for x in [0.0, 1.0]]
		pad_pos_y_pixel = [plotData.plot.plot_pad.VtoPixel(y) for y in [0.0, 1.0]]
		canvas_pos_x_pixel = [plotData.plot.canvas.UtoPixel(x) for x in [0.0, 1.0]]
		canvas_pos_y_pixel = [plotData.plot.canvas.VtoPixel(y) for y in [0.0, 1.0]]
		legend_pos_x_pixel = [int(pad_pos_x_pixel[0] + (x * (pad_pos_x_pixel[1] - pad_pos_x_pixel[0]))) for x in plotData.plotdict["legend"][::2]]
		legend_pos_y_pixel = [int(pad_pos_y_pixel[0] + (y * (pad_pos_y_pixel[1] - pad_pos_y_pixel[0]))) for y in plotData.plotdict["legend"][1::2]]
		legend_pos_x_user = [float(x - canvas_pos_x_pixel[0]) / float(canvas_pos_x_pixel[1] - canvas_pos_x_pixel[0]) for x in legend_pos_x_pixel]
		legend_pos_y_user = [float(y - canvas_pos_y_pixel[0]) / float(canvas_pos_y_pixel[1] - canvas_pos_y_pixel[0]) for y in legend_pos_y_pixel]
		transformed_legend_pos = tools.flattenList(zip(legend_pos_x_user, legend_pos_y_user))
		"""
		transformed_legend_pos = plotData.plotdict["legend"]
		
		plotData.plot.plot_pad.cd()
		self.legend = None
		if plotData.plotdict["legend"] != None:
			ROOT.gStyle.SetLegendBorderSize(0)
			self.legend = ROOT.TLegend(*transformed_legend_pos)
			self.legend.SetNColumns(plotData.plotdict["legend_cols"])
			self.legend.SetColumnSeparation(0.1)
			
			for subplot, nick, label, legend_marker in zip(
					plotData.plotdict["subplots"],
					plotData.plotdict["nicks"],
					plotData.plotdict["labels"],
					plotData.plotdict["legend_markers"],
			):
				if subplot == True:
					# TODO handle possible subplot legends
					continue
				root_object = plotData.plotdict["root_objects"][nick]
				if legend_marker is None:
					# TODO: defaults should be defined in prepare_args function
					legend_marker = "FLP"
					if isinstance(root_object, ROOT.TH1):
						legend_marker = "F"
					elif isinstance(root_object, ROOT.TGraph):
						legend_marker = "LP"
					elif isinstance(root_object, ROOT.TF1):
						legend_marker = "L"
				if (not label is None) and (label != ""):
					self.legend.AddEntry(root_object, label, legend_marker)
			
			defaultrootstyle.set_legend_style(self.legend)
			self.legend.Draw()
	
	def add_texts(self, plotData):
		super(PlotRoot, self).add_texts(plotData)

		self.text_box = ROOT.TPaveText(0.0, 0.0, 1.0, 1.0, "NDC")
		self.text_box.SetFillStyle(0)
		self.text_box.SetBorderSize(0)
		self.text_box.SetShadowColor(0)
		self.text_box.SetTextAlign(22)

		text_size = self.axes_histogram.GetXaxis().GetLabelSize()
		if not self.subplot_axes_histogram is None:
			text_size = self.subplot_axes_histogram.GetXaxis().GetLabelSize() * (1-self.plot_subplot_slider_y-0.12)
		self.text_box.SetTextSize(text_size)

		for x, y, text, size in zip(plotData.plotdict["texts_x"], plotData.plotdict["texts_y"], plotData.plotdict["texts"], plotData.plotdict["texts_size"]):
			text_object = self.text_box.AddText(x, y, text)
			if not size is None:
				text_object.SetTextSize(size)

		# lumi and energy: outside plot, top right, with best possible offset
		if self.dataset_title != "":
			self.dataset_title = re.sub(r"\\mathrm{(fb|pb)}", re.search(r"\\mathrm{(fb|pb)}", self.dataset_title).group(1), self.dataset_title)
			year = "("
			if plotData.plotdict["year"] != "":
				year += plotData.plotdict["year"] + ", "
			CMS_lumi.lumi_sqrtS = self.dataset_title.replace("$", "").replace("\,", "").split("(")[0] + year + self.dataset_title.replace("$", "").replace("\,", "").split("(")[1]
			CMS_lumi.lumiTextSize = 0.5
			if not self.subplot_axes_histogram is None:
				CMS_lumi.lumiTextOffset = 0.4

		# normal plot title (e.g., 'own work', name of the channel...): outside plot, top left
		y_title = 0.95 if self.subplot_axes_histogram is None else 0.923
		if (not plotData.plotdict["title"] is None) and (plotData.plotdict["title"] != ""):
			x_title = 0.2
			title = self.text_box.AddText(x_title, y_title, plotData.plotdict["title"])
			title.SetTextAlign(11)

		# CMS text (only if specified): inside plot, top left
		CMS_lumi.cmsTextSize = 0.5
		if not (plotData.plotdict["cms"] or plotData.plotdict["cms_outframe"]):
			CMS_lumi.cmsText = ""

		CMS_lumi.extraText = plotData.plotdict["extra_text"]
		if plotData.plotdict["cms_outframe"]:
			CMS_lumi.relPosX = 0.12
			CMS_lumi.CMS_lumi(plotData.plot.canvas, 0, 0)
		else:
			CMS_lumi.CMS_lumi(plotData.plot.canvas, 0, 11)

		# Draw the text box on the main plot in plot_pad
		plotData.plot.plot_pad.cd()
		self.text_box.Draw()
	
	@staticmethod
	def _set_axis_limits(pad, root_object, max_dim, x_lims=None, y_lims=None, z_lims=None, reverse_x_axis=False, reverse_y_axis=False, reverse_z_axis=False):
		""" not needed here due to axis histogram
		if not x_lims is None:
			root_object.GetXaxis().SetRangeUser(*x_lims)
			root_object.GetXaxis().SetLimits(*x_lims)
		
		if (max_dim > 1) and (not y_lims is None):
			root_object.GetYaxis().SetRangeUser(*y_lims)
			root_object.GetYaxis().SetLimits(*y_lims)
		"""
		
		if (max_dim > 2) and (not z_lims is None):
			try:
				root_object.GetZaxis().SetRangeUser(*z_lims)
				root_object.GetZaxis().SetLimits(*z_lims)
				#palette = root_object.GetListOfFunctions().FindObject("palette")
				#if palette != None:
				#	palette.GetAxis().SetRangeUser(*z_lims)
				#	palette.GetAxis().SetLimits(*z_lims)
			except:
				#catch AttributeError for ROOT objects having lower dimensions
				pass
		
		if reverse_x_axis:
			pad.Update()
			reversed_x_axis = ROOT.TGaxis(
					pad.GetUxmax(),
					pad.GetUymin(),
					pad.GetUxmin(),
					pad.GetUymin(),
					-root_object.GetXaxis().GetXmax(),
					-root_object.GetXaxis().GetXmin(),
					root_object.GetXaxis().GetNdivisions(),
					"-"
			)
			
			reversed_x_axis.ImportAxisAttributes(root_object.GetXaxis())
			reversed_x_axis.SetLabelOffset((-7.5)*root_object.GetXaxis().GetLabelOffset())
			
			root_object.GetXaxis().SetLabelSize(0.0)
			reversed_x_axis.SetTitleSize(0.0)
			
			reversed_x_axis.Draw()
			return reversed_x_axis
		
		if reverse_y_axis or reverse_z_axis:
			#TODO: https://root.cern.ch/root/html/tutorials/hist/reverseaxis.C.html#38
			log.warning("Reversing the Y/Z-axis is not yet implemented")

