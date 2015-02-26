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
import Artus.HarryPlotter.utility.extrafunctions as extrafunctions

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
		self.formatting_options.add_argument("--fill-styles", type=int, nargs="+",
		                                     help="Fill styles for histograms. Defaults choosen according to draw options.")
		self.formatting_options.add_argument("--line-styles", nargs="+", default=[1], type=int,
                                             help="Line style of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--legend", type=float, nargs=2, default=None,
		                                     help="Location of the legend. Use 'None' to not set any legend. [Default: %(default)s]")
		self.formatting_options.add_argument("--stacks-errband", action='store_true', default=False,
		                                     help="Draw error band on top of the stacked plots.")
		self.formatting_options.add_argument("--stacks-errband-names", nargs="+",
		                                     help="Nick names for stacked plots for which an error band will be shown. [Default: all but first stack nick]")
		
	def prepare_args(self, parser, plotData):
		super(PlotRoot, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["nicks", "colors", "labels", "markers", "line_styles", "x_errors", "y_errors", "stacks", "axes",
		                                  "fill_styles"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks", "stacks"] if plotData.plotdict[l] is not None]))
		
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
		for index, (marker, fill_style, stack) in enumerate(zip(plotData.plotdict["markers"],
		                                                        plotData.plotdict["fill_styles"],
		                                                        plotData.plotdict["stacks"])):
			if marker is None:
				if index == 0:
					marker = "E" if len(plotData.plotdict["markers"]) > 1 else "HIST"
				else:
					marker = "LINE" if plotData.plotdict["stacks"].count(stack) == 1 else "HIST"
				# TODO: defaults for 2D/3D histograms
			
			if fill_style is None:
				fill_style = 0
				if "HIST" in marker.upper():
					fill_style = 1001
				elif "LINE" in marker.upper():
					marker = marker.upper().replace("LINE", "HIST")
					fill_style = 0
				elif "E" in marker.upper():
					fill_style = 3003
			
			plotData.plotdict["markers"][index] = marker
			plotData.plotdict["fill_styles"][index] = fill_style
		
		# defaults for stacked plots shaded error band
		if plotData.plotdict["stacks_errband_names"] == None: plotData.plotdict["stacks_errband_names"] = [" ".join(set(plotData.plotdict["stacks"][1:]))]
		plotData.plotdict["stacks_errband_names"] = [nicks.split() for nicks in plotData.plotdict["stacks_errband_names"]]
		for nicks in plotData.plotdict["stacks_errband_names"]:
			for nick in nicks:
				if nick not in plotData.plotdict["stacks"]:
					log.critical("Stack name \"%s\" of argument --%s does not exist in argument --stack!" % (nick, "stacks_errband_names".replace("_", "-")))
					sys.exit(1)
	
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
		
		if self.canvas == None:
			self.canvas = ROOT.TCanvas()

		#Modify right side margin for 2d-plots
		if not isinstance(plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]], ROOT.TGraph):
			if plotData.plotdict["root_objects"][plotData.plotdict["nicks"][0]].GetDimension() == 2:
				self.canvas.SetRightMargin(0.15)

		if (plotData.plotdict["ratio"] == True):
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
		
		for nick, color, marker, fill_style, line_style in zip(plotData.plotdict["nicks"],
		                                                       plotData.plotdict["colors"],
		                                                       plotData.plotdict["markers"],
		                                                       plotData.plotdict["fill_styles"],
		                                                       plotData.plotdict["line_styles"]):
			root_object = plotData.plotdict["root_objects"][nick]
			if plotData.plotdict["title"]:
				root_object.SetTitle(plotData.plotdict["title"])
			
			root_object.SetLineColor(color)
			root_object.SetLineStyle(line_style)
			
			root_object.SetMarkerColor(color)
			root_object.SetMarkerStyle(0 if "E2" in marker.upper() and fill_style > 3000 else 20)
			
			root_object.SetFillColor(color)
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
			root_object = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][plot_index]]
			
			marker = plotData.plotdict["markers"][plot_index]
			draw_option = marker + ("" if index == 0 else " SAME")
			
			axes_histogram = None
			if isinstance(root_object, ROOT.TH1):
				axes_histogram = self._draw_histogram(self.plot_pad, root_object, draw_option, set_first_plotted_histogram=(index == 0))
			elif isinstance(root_object, ROOT.TGraph):
				axes_histogram = self._draw_graph(self.plot_pad, root_object, draw_option+(" A" if index == 0 else ""), set_first_plotted_histogram=(index == 0))
			else:
				log.error("ROOT plotting is not (yet) implemented for objects of type %s!" % type(root_object))
			
			if axes_histogram:
				# trace min. and max. values of axes
				if axes_histogram.GetXaxis().GetXmin() < self.x_min: self.x_min = axes_histogram.GetXaxis().GetXmin()
				if axes_histogram.GetXaxis().GetXmax() > self.x_max: self.x_max = axes_histogram.GetXaxis().GetXmax()
				if axes_histogram.GetDimension() > 1:
					if axes_histogram.GetYaxis().GetXmin() < self.y_min: self.y_min = axes_histogram.GetYaxis().GetXmin()
					if axes_histogram.GetYaxis().GetXmax() > self.y_max: self.y_max = axes_histogram.GetYaxis().GetXmax()
				else:
					if axes_histogram.GetMinimum() < self.y_min: self.y_min = axes_histogram.GetMinimum()
					if axes_histogram.GetMaximum() > self.y_max: self.y_max = axes_histogram.GetMaximum()
				if axes_histogram.GetDimension() > 2:
					if axes_histogram.GetZaxis().GetXmin() < self.z_min: self.z_min = axes_histogram.GetZaxis().GetXmin()
					if axes_histogram.GetZaxis().GetXmax() > self.z_max: self.z_max = axes_histogram.GetZaxis().GetXmax()
				else:
					if axes_histogram.GetMinimum() < self.z_min: self.z_min = axes_histogram.GetMinimum()
					if axes_histogram.GetMaximum() > self.z_max: self.z_max = axes_histogram.GetMaximum()
		
		# draw shaded error band for the chosen stacked histograms
		if (plotData.plotdict["stacks_errband"] == True):
			for nicks in plotData.plotdict["stacks_errband_names"]:
				for nick in nicks:
					plotData.plotdict["root_stack_histos"][nick].SetMarkerStyle(1)
					plotData.plotdict["root_stack_histos"][nick].SetFillColor(1)
					plotData.plotdict["root_stack_histos"][nick].SetFillStyle(3001)
					plotData.plotdict["root_stack_histos"][nick].Draw("e2 same")
		
		if (plotData.plotdict["ratio"] == True):
			self.ratio_pad.cd()
			
			self.ratio_y_min = sys.float_info.max
			self.ratio_y_max = -sys.float_info.max
		
			for index, root_object in enumerate(plotData.plotdict["root_ratio_histos"]):
				option = "e" if index == 0 else "e2 same"
				if index == 0: self.first_plotted_ratio_histogram = root_object
				else:
					root_object.SetMarkerStyle(1)
					root_object.SetFillColor(1)
					root_object.SetFillStyle(3001)
				root_object.Draw(option)
			
				# trace min. and max. values of axes
				if root_object.GetMinimum() < self.ratio_y_min: self.ratio_y_min = root_object.GetMinimum()
				if root_object.GetMaximum() > self.ratio_y_max: self.ratio_y_max = root_object.GetMaximum()
	
	def _draw_histogram(self, pad, root_histogram, draw_option, set_first_plotted_histogram=False):
		assert isinstance(root_histogram, ROOT.TH1)
		pad.cd()
		root_histogram.Draw(draw_option)
		if set_first_plotted_histogram:
			self.first_plotted_histogram = root_histogram
		return root_histogram
	
	def _draw_graph(self, pad, root_graph, draw_option, set_first_plotted_histogram=False):
		assert isinstance(root_graph, ROOT.TGraph)
		pad.cd()
		root_graph.Draw(draw_option)
		root_histogram = root_graph.GetHistogram()
		if set_first_plotted_histogram:
			self.first_plotted_histogram = root_histogram
		return root_histogram
	
	def modify_axes(self, plotData):
		super(PlotRoot, self).modify_axes(plotData)
	
		# axis labels
		self.first_plotted_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
		self.first_plotted_histogram.GetYaxis().SetTitle(plotData.plotdict["y_label"])
		self.first_plotted_histogram.GetZaxis().SetTitle(plotData.plotdict["z_label"])
		if (plotData.plotdict["ratio"] == True):
			self.first_plotted_ratio_histogram.GetXaxis().SetTitle(plotData.plotdict["x_label"])
			self.first_plotted_ratio_histogram.GetYaxis().SetTitle(plotData.plotdict["y_ratio_label"])
	
		# logaritmic axis
		if plotData.plotdict["x_log"]: self.plot_pad.SetLogx()
		if plotData.plotdict["y_log"]: self.plot_pad.SetLogy()
		if plotData.plotdict["z_log"]: self.plot_pad.SetLogz()
		if (plotData.plotdict["ratio"] == True):
			if plotData.plotdict["x_log"]: self.ratio_pad.SetLogx()
	
		# axis limits
		if plotData.plotdict["x_lims"] == None:
			self.first_plotted_histogram.GetXaxis().SetRangeUser(self.x_min,
			                                                     self.x_max)
			self.first_plotted_histogram.GetXaxis().SetLimits(self.x_min,
			                                                  self.x_max)
			if (plotData.plotdict["ratio"] == True):
				self.first_plotted_ratio_histogram.GetXaxis().SetRangeUser(self.x_min,
				                                                           self.x_max)
				self.first_plotted_ratio_histogram.GetXaxis().SetLimits(self.x_min,
				                                                        self.x_max)
		else:
			self.first_plotted_histogram.GetXaxis().SetRangeUser(plotData.plotdict["x_lims"][0],
			                                                     plotData.plotdict["x_lims"][1])
			self.first_plotted_histogram.GetXaxis().SetLimits(plotData.plotdict["x_lims"][0],
			                                                  plotData.plotdict["x_lims"][1])
			if (plotData.plotdict["ratio"] == True):
				self.first_plotted_ratio_histogram.GetXaxis().SetRangeUser(plotData.plotdict["x_lims"][0],
				                                                           plotData.plotdict["x_lims"][1])
				self.first_plotted_ratio_histogram.GetXaxis().SetLimits(plotData.plotdict["x_lims"][0],
				                                                        plotData.plotdict["x_lims"][1])
		if plotData.plotdict["y_lims"] == None:
			self.first_plotted_histogram.GetYaxis().SetRangeUser(self.y_min,
			                                                     self.y_max * 1.1)
			self.first_plotted_histogram.GetYaxis().SetLimits(self.y_min,
			                                                  self.y_max * 1.1)
		else:
			self.first_plotted_histogram.GetYaxis().SetRangeUser(plotData.plotdict["y_lims"][0],
			                                                     plotData.plotdict["y_lims"][1])
			self.first_plotted_histogram.GetYaxis().SetLimits(plotData.plotdict["y_lims"][0],
			                                                  plotData.plotdict["y_lims"][1])
		if (plotData.plotdict["ratio"] == True):
			if plotData.plotdict["y_ratio_lims"] == None:
				self.first_plotted_ratio_histogram.GetYaxis().SetRangeUser(self.ratio_y_min,
				                                                           self.ratio_y_max * 1.1)
				self.first_plotted_ratio_histogram.GetYaxis().SetLimits(self.ratio_y_min,
				                                                        self.ratio_y_max * 1.1)
			else:
				self.first_plotted_ratio_histogram.GetYaxis().SetRangeUser(plotData.plotdict["y_ratio_lims"][0],
				                                                           plotData.plotdict["y_ratio_lims"][1])
				self.first_plotted_ratio_histogram.GetYaxis().SetLimits(plotData.plotdict["y_ratio_lims"][0],
				                                                        plotData.plotdict["y_ratio_lims"][1])
		if plotData.plotdict["z_lims"] == None:
			self.first_plotted_histogram.GetZaxis().SetRangeUser(self.z_min,
			                                                     self.z_max)
			self.first_plotted_histogram.GetZaxis().SetLimits(self.z_min,
			                                                  self.z_max)
		else:
			self.first_plotted_histogram.GetZaxis().SetRangeUser(plotData.plotdict["z_lims"][0],
			                                                     plotData.plotdict["z_lims"][1])
			self.first_plotted_histogram.GetZaxis().SetLimits(plotData.plotdict["z_lims"][0],
			                                                  plotData.plotdict["z_lims"][1])
		
		if (plotData.plotdict["ratio"] == True):
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
		
		# redraw axes only and update the canvas
		self.plot_pad.cd()
		self.first_plotted_histogram.Draw("AXIS SAME")
		self.plot_pad.Update()
		
		if not self.ratio_pad is None:
			self.ratio_pad.cd()
			if not self.first_plotted_ratio_histogram is None:
				self.first_plotted_ratio_histogram.Draw("AXIS SAME")
			self.ratio_pad.Update()
		
		self.canvas.Update()
			
		
	def add_grid(self, plotData):
		super(PlotRoot, self).add_grid(plotData)
		
		self.plot_pad.cd()
		if (plotData.plotdict["grid"] or plotData.plotdict["x_grid"]):
			self.plot_pad.SetGridx()
		if (plotData.plotdict["grid"] or plotData.plotdict["y_grid"]):
			self.plot_pad.SetGridy()
		
		if (plotData.plotdict["ratio"] == True):
			self.ratio_pad.cd()
			if (plotData.plotdict["ratio_grid"] == True):
				self.ratio_pad.SetGrid()
	
	def add_labels(self, plotData):
		super(PlotRoot, self).add_labels(plotData)
		
		self.plot_pad.cd()
		self.legend = None
		if plotData.plotdict["legend"] != None:
			self.legend = ROOT.TLegend(plotData.plotdict["legend"][0], plotData.plotdict["legend"][1], 0.9, 0.9);
			for plot_index in self.plot_sequence_indices[::-1]:
				root_object = plotData.plotdict["root_objects"][plotData.plotdict["nicks"][plot_index]]
				marker = plotData.plotdict["markers"][plot_index]
				label = plotData.plotdict["labels"][plot_index]
				if label != None:
					self.legend.AddEntry(root_object, label, "LP" if "e" in marker.lower() else "F")
			self.legend.Draw()
	
	def add_texts(self, plotData):
		super(PlotRoot, self).add_texts(plotData)
		if plotData.plotdict["texts"] != [None]:
			self.plot_pad.cd()
			for x, y, text in zip(plotData.plotdict['texts_x'], plotData.plotdict['texts_y'], plotData.plotdict['texts']):
				root_text = ROOT.TPaveText(float(x), float(y), 0.9, 0.9, "NDC")
				root_text.AddText(text)
				root_text.SetLineColor(0)
				root_text.SetFillColor(0)
				root_text.SetShadowColor(0)
				root_text.Draw()
	
	def save_canvas(self, plotData):
		super(PlotRoot, self).save_canvas(plotData)
		
		self.canvas.RedrawAxis()
		if self.plot_pad:
			self.plot_pad.RedrawAxis()
		if self.ratio_pad:
			self.ratio_pad.RedrawAxis()
		
		for output_filename in plotData.plotdict["output_filenames"]:
			self.canvas.SaveAs(output_filename)
			log.info("Created plot \"%s\"." % output_filename)

