#  -*- coding: utf-8 -*-

import os
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata

from Artus.HarryPlotter.utility.mplhisto import MplHisto, MplGraph
import Artus.HarryPlotter.utility.labels as labels

import array
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize
from mpl_toolkits.mplot3d import Axes3D

from itertools import cycle

import numpy as np


class MplPlotContainer(plotdata.PlotContainer):
	def __init__(self, fig=None, axes=None, save_legend=False, labels=None):
		self.fig = fig
		self.axes = axes
		self.save_legend = save_legend
		self.legend_fig = None
		self.labels = labels
	
	def finish(self):
		if self.save_legend != False:
			self.legend_fig = plt.figure()
			legend_ax = self.legend_fig.add_subplot(111, frameon=False)
			legend_ax.xaxis.set_visible(False)
			legend_ax.yaxis.set_visible(False)
			handles, labels = PlotMpl.get_legend_handles_labels_ordered(self.axes[0], self.labels)
			legend_ax.legend(handles, labels, loc='center', frameon=False)

	def save(self, filename):
		# use bbox tight and pad to not have labels cropped
		# TODO should we use fixed bbox values?
		self.fig.savefig(filename, pad_inches=0.1, **({'bbox_inches': 'tight'} if '.pgf' not in filename else {}))
		if self.save_legend != False:
			legend_filename = os.path.join(os.path.dirname(filename), self.save_legend+os.path.splitext(filename)[-1])
			self.legend_fig.savefig(
					legend_filename,
					# TODO cleanly crop figure to legend size
					# the arguments below are just workarounds :(
					bbox_inches='tight',
					pad_inches=0.1
			)
			log.info("Legend saved to " + legend_filename)

class PlotMpl(plotbase.PlotBase):
	"""Create scientific plots using the Matplotlib backend."""

	def __init__(self):
		super(PlotMpl, self).__init__()
		self.mpl_version = int(matplotlib.__version__.replace(".", ""))
		self.default_bar_colors = [
			(0.21568627450980393, 0.49411764705882355, 0.7215686274509804),  # blue
			(0.30196078431372547, 0.6862745098039216, 0.2901960784313726),  # green
			(0.596078431372549, 0.3058823529411765, 0.6392156862745098), # violet
			(1.0, 0.4980392156862745, 0.0),  # orange
			(1.0, 1.0, 0.2),  # yellow
			(0.6509803921568628, 0.33725490196078434, 0.1568627450980392),  # brown
			(0.9686274509803922, 0.5058823529411764, 0.7490196078431373),  # pink
			(0.6, 0.6, 0.6)  # grey
		]
		self.default_marker_colors = ['black', 'red', 'blue', 'green', 'purple', 'orange', 'cyan',
			'magenta', 'gold', 'saddlebrown', 'olivedrab', 'lightsteelblue', 'burlywood', 'lightcoral']
		self.set_matplotlib_defaults()
		self.nicelabels = labels.LabelsDict()
		self.default_linestyles = ['--', '-.', ':', '-']
		self.rel_y_lim_default['lin']=[0.9, 1.2]
		self.plot_dimension = 0


	def modify_argument_parser(self, parser, args):
		super(PlotMpl, self).modify_argument_parser(parser, args)
		self.formatting_options.add_argument("--x-errors", type='bool', nargs="+",
		                                     help="Show x errors for the nicks. [Default: True for first plot, False otherwise]")
		self.formatting_options.add_argument("--y-errors", type='bool', nargs="+",
		                                     help="Show y errors for the plots. [Default: True for first plot, False otherwise]")
		self.formatting_options.add_argument("--edgecolors", nargs="+",
		                                     help="Edgecolor to be passed to plot objects.")
		self.formatting_options.add_argument("--step", default=False, type='bool', nargs="+",
		                                     help="Step lines according to bin edges instead of connecting points.")
		self.formatting_options.add_argument("--zorder", type=int, nargs="+",
		                                     help="Order in which plots are layered. Default is first nick always on top, then zorder increases.")
		self.formatting_options.add_argument("--3d", type=int, nargs="?", default=False, const=0,
		                                     help="If set, a 2D histogram is plotted in 3D. Optional Argument is the viewing angle. Default: %(default)s")
		self.formatting_options.add_argument("--save-legend", type=str, nargs="?", default=False, const="legend",
		                                     help="If set, the legend is saved as a separate file. Argument is the filename. Default: %(default)s")
		self.formatting_options.add_argument("--marker-fill-styles", type=str, nargs="+",
		                                     help="Fill style of the markers in the plot")
		self.formatting_options.add_argument("--subplot-fraction", type=int, nargs="?", default=25,
											 help="Hight fraction of the subplot in percent")
		self.formatting_options.add_argument("--subplot-legend", type=str, nargs="?", default=None,
		                                     help="Location of the subplot legend. Use 'None' to not set any legend")
		self.formatting_options.add_argument("--alphas", type=float, nargs="+", default=[1.0],
		                                     help="Alpha (opacity) values for bands, shaded areas etc. 0.0 -> fully transparent. [Default: %(default)s]")
		self.formatting_options.add_argument("--rasterized", default=False, action='store_true',
		                                     help="Rasterize color mesh and bar in 2D plots.")

		self.formatting_options.set_defaults(legend="upper right")
		self.formatting_options.set_defaults(colormap="afmhot")


	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)

		self.prepare_list_args(
				plotData,
				["nicks", "colors", "labels", "markers", "marker_fill_styles", "line_styles", "line_widths", "x_errors", "y_errors", "step", "zorder", "edgecolors", "alphas"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks", "stacks"] if plotData.plotdict[l] is not None]
		))

		for index, error in enumerate(plotData.plotdict["x_errors"]):
			if error is None:
				plotData.plotdict["x_errors"][index] = True if index == 0 else False
		for index, error in enumerate(plotData.plotdict["y_errors"]):
			if error is None:
				plotData.plotdict["y_errors"][index] = True if index == 0 else False

		# if plotData.plotdict['subplot_nicks'] != []):
			# self.set_default_ratio_colors(plotData)

		# defaults for markers
		for index, (marker, nick) in enumerate(zip(plotData.plotdict["markers"], plotData.plotdict["nicks"])):
			if marker is None:
				if ((index == 0 and len(plotData.plotdict["markers"]) > 1)
						or plotData.plotdict['subplots'][index]
						or 'ratio' in nick
						or isinstance(plotData.plotdict["root_objects"][nick], ROOT.TProfile)
						or isinstance(plotData.plotdict["root_objects"][nick], ROOT.TGraph)):
					plotData.plotdict["markers"][index] = "."
				else:
					plotData.plotdict["markers"][index] = "fill"
					
		# defaults for axis ranges
		if plotData.plotdict["y_rel_lims"] is None:
			plotData.plotdict["y_rel_lims"] = [0.5, 2.0] if plotData.plotdict["y_log"] else [0.9, 1.2]

		# set default colors depending whether there are markers or bars
		i_marker, i_bar = 0, 0
		for marker_index, marker in enumerate(plotData.plotdict['markers']):
			if plotData.plotdict['colors'][marker_index] == None:
				if marker in ['bar', 'fill']:
					plotData.plotdict['colors'][marker_index] = self.default_bar_colors[i_bar % len(self.default_bar_colors)]
					i_bar += 1
				else:
					plotData.plotdict['colors'][marker_index] = self.default_marker_colors[i_marker % len(self.default_marker_colors)]
					i_marker += 1

		for index, marker in enumerate(plotData.plotdict["zorder"]):
			if marker is None:
				if index == 0:
					plotData.plotdict["zorder"][index] = 99
				else:
					plotData.plotdict["zorder"][index] = index

		# remove escape slashes
		for index, line_style in enumerate(plotData.plotdict["line_styles"]):
			plotData.plotdict["line_styles"][index] = line_style.replace("\\", "") if line_style else line_style

		# default for line styles
		for index, line_style in enumerate(plotData.plotdict["line_styles"]):
			if line_style is None:
				plotData.plotdict["line_styles"][index] = ""

		# default for line widths
		for index, line_width in enumerate(plotData.plotdict["line_widths"]):
			if line_width is None:
				plotData.plotdict["line_widths"][index] = 2

		# remove escape slashes
		for index, marker_fill_style in enumerate(plotData.plotdict["marker_fill_styles"]):
			plotData.plotdict["marker_fill_styles"][index] = marker_fill_style.replace("\\", "") if marker_fill_style else marker_fill_style

		# default for marker fill styles
		for index, marker_fill_style in enumerate(plotData.plotdict["marker_fill_styles"]):
			if marker_fill_style is None:
				plotData.plotdict["marker_fill_styles"][index] = "full"

		# validate length of parameters first
		zip_arguments = self.get_zip_arguments(plotData)
		for argument in zip_arguments:
			if len(argument) != len(zip_arguments[0]):
				log.warning("The PlotMpl module is trying to make plots with invalid inputs. The Plot will eventually not contain all requested information.")
				log.warning("This the invalid input: %s" % argument)
				break


		# check subplot-fraction range
		if plotData.plotdict["subplot_fraction"] > 100:
			plotData.plotdict["subplot_fraction"] = 100
		elif plotData.plotdict["subplot_fraction"] < 0:
			plotData.plotdict["subplot_fraction"] = 0

	def run(self, plotData):
		super(PlotMpl, self).run(plotData)

	def set_style(self, plotData):
		super(PlotMpl, self).set_style(plotData)
	
	def create_canvas(self, plotData):
		super(PlotMpl, self).create_canvas(plotData)
		
		# fig = plt.figure(figsize = [
			# plotData.plotdict['axes_layout'][0] * matplotlib.rcParams['figure.figsize'][0],
			# plotData.plotdict['axes_layout'][1] * matplotlib.rcParams['figure.figsize'][1]
			# ])
		fig = plt.figure()
		axes = []
		if plotData.plotdict["subplot_nicks"]:
			axes = [plt.subplot2grid((100,1), (0, 0), rowspan=(100-plotData.plotdict["subplot_fraction"])),
			        plt.subplot2grid((100,1), ((100-plotData.plotdict["subplot_fraction"]+4), 0), rowspan=plotData.plotdict["subplot_fraction"])]
		else:
			kwargs = {'projection':'3d'} if (plotData.plotdict['3d'] is not False) else {}
			axes = [fig.add_subplot(1,1,1, **kwargs)]

		plotData.plot = MplPlotContainer(fig, axes, plotData.plotdict["save_legend"], plotData.plotdict["labels"])

	def prepare_histograms(self, plotData):
		super(PlotMpl, self).prepare_histograms(plotData)


	def make_plots(self, plotData):

		zip_arguments = self.get_zip_arguments(plotData)

		for nick, color, edgecolor, label, marker, marker_fill_style, x_error, y_error, line_style, line_width, step, zorder, alpha in zip(*zip_arguments):
			if nick in plotData.plotdict["subplot_nicks"]:
				ax = plotData.plot.axes[1]
			else:
				ax = plotData.plot.axes[0]
			log.info("Plotting nick '{0}' with label '{1}'".format(nick, label))
			root_object = plotData.plotdict["root_objects"][nick]

			if isinstance(root_object, ROOT.TGraph):
				self.plot_dimension = max([1, self.plot_dimension])
				self.mplhist = MplGraph(root_object)

				if marker=='fill':
					self.plot_tgrapherrors_envelope(self.mplhist, ax, label, color, line_style, line_width, step, zorder, alpha)
				else:
					self.plot_errorbar(self.mplhist, ax=ax,
					               show_xerr=x_error, show_yerr=y_error,
					               color=color, fmt=marker, marker_fill_style=marker_fill_style, capsize=0,
					               linestyle=line_style, linewidth=line_width, label=label, zorder=zorder)

			elif isinstance(root_object, ROOT.TH2):
				self.mplhist = MplHisto(root_object)
				if plotData.plotdict['3d'] is not False:
					self.plot_dimension = max([3, self.plot_dimension])
				else:
					self.plot_dimension = max([self.mplhist.dimension, self.plot_dimension])
				if plotData.plotdict["z_lims"]:
					vmin = plotData.plotdict["z_lims"][0]
					vmax = plotData.plotdict["z_lims"][1]
				# elif plotData.plotdict["z_log"]:
				# 	vmin = 1
				# 	vmax = None
				else:
					vmin = None
					vmax = None
				cmap = plt.cm.get_cmap(plotData.plotdict["colormap"])

				if plotData.plotdict["3d"] is not False:
					self.image = self.plot_3d(self.mplhist, ax=ax, cmap=cmap, angle=plotData.plotdict["3d"])
				else:
					self.image = self.plot_contour1d(self.mplhist, ax=ax, vmin=vmin, vmax=vmax, z_log=plotData.plotdict["z_log"], cmap=cmap, rasterized=plotData.plotdict["rasterized"])

			elif isinstance(root_object, ROOT.TH1):
				self.mplhist = MplHisto(root_object)
				self.plot_dimension = max([self.plot_dimension, self.mplhist.dimension])

				if marker in ["bar", "fill"]:
					self.plot_hist1d(self.mplhist, style=marker, ax=ax, show_yerr=y_error, label=label, color=color, edgecolor=edgecolor, alpha=alpha, zorder=zorder)
				else:
					self.plot_errorbar(self.mplhist, ax=ax,
					                   show_xerr=x_error, show_yerr=y_error,
					                   step=step, color=color, fmt=marker, marker_fill_style=marker_fill_style,
					                   capsize=2, linestyle=line_style, linewidth=line_width, label=label, zorder=zorder)

			# draw functions from dictionary
			elif isinstance(root_object, ROOT.TF1):
				x_values = []
				y_values = []
				sampling_points = 1000
				x_range = [ root_object.GetXaxis().GetXmin(), root_object.GetXaxis().GetXmax()] 
				for x in np.arange(x_range[0], x_range[1], (float(x_range[1])-float(x_range[0]))/sampling_points):
					x_values.append(x)
					y_values.append(root_object.Eval(x))
				ax.plot(x_values, y_values, label=label, color=color, linestyle=line_style, linewidth=line_width)

				if y_error and nick in plotData.fit_results:
					x = array.array('d', x_values)
					y = array.array('d', [0.]*len(x_values))
					plotData.fit_results[nick].GetConfidenceIntervals(len(x_values), 1, 1, x, y, 0.683)
					conf_intervals = [i for i in y]

					ax.fill_between(x_values,
									[(y_val-c) for y_val, c in zip(y_values, conf_intervals)],
									[(y_val+c) for y_val, c in zip(y_values, conf_intervals)],
									facecolor=color,
									edgecolor=color,
									interpolate=True,
									alpha=alpha)

	def modify_axes(self, plotData):
		# do what is needed for all plots:
		super(PlotMpl, self).modify_axes(plotData)

		# Main axis
		ax = plotData.plot.axes[0]
		if plotData.plotdict["grid"]:
			{'horizontal': ax.yaxis, 'vertical':ax.xaxis}.get(plotData.plotdict["grid"], ax).grid(True)
		# set axis labels
		if plotData.plotdict['subplot_nicks'] == []:
			ax.set_xlabel(self.nicelabels.get_nice_label(plotData.plotdict["x_label"]), position=(1., 0.), va='top', ha='right')
		ax.set_ylabel(self.nicelabels.get_nice_label(plotData.plotdict["y_label"]), position=(0., 1.), va='top', ha='right')

		#display 10^N instead of 1eN
		for axis in ["xaxis", "yaxis"]:
			getattr(ax, axis).major.formatter._useMathText = True

		# set axis ticks
		if plotData.plotdict["x_ticks"] is not None and not (hasattr(self.mplhist, "xlabels") and self.mplhist.xlabels is not None):
			ax.set_xticks(plotData.plotdict["x_ticks"])
		elif hasattr(self.mplhist, "xlabels") and self.mplhist.xlabels is not None:
			plt.xticks(np.arange(len(self.mplhist.x)), self.mplhist.xlabels, rotation='75')
		if plotData.plotdict["y_ticks"] is not None:
			ax.set_yticks(plotData.plotdict["y_ticks"])
		axis = 'both' if (not hasattr(self.mplhist, "xlabels") or self.mplhist.xlabels == None) else 'y'
		ax.ticklabel_format(style='sci',scilimits=(-3,4),axis=axis)
		# ticklabels
		for axis, axisname in zip([ax.xaxis, ax.yaxis], ['x', 'y']):
			if plotData.plotdict[axisname+"_tick_labels"] is not None:
				axis.set_ticklabels(plotData.plotdict[axisname+"_tick_labels"], va=("baseline" if axisname=='x' else "center"))
		if (not plotData.plotdict["subplot_nicks"]) and plotData.plotdict["x_tick_labels"] and (not all([s.isdigit() for s in plotData.plotdict["x_tick_labels"]])):
			ax.get_xaxis().set_tick_params(pad=16)  # extra padding if ticklabels arent numbers

		# set axis limits
		if self.plot_dimension != 3:
			if plotData.plotdict["x_lims"] is not None:
				ax.set_xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
			else:
				# iterate over root objects and get min and max x values
				min_x_values, max_x_values = [], []
				for nick in plotData.plotdict['nicks']:
					if hasattr(plotData.plotdict['root_objects'][nick], "GetNbinsX"):  # for Histograms
						min_x_values.append(plotData.plotdict['root_objects'][nick].GetXaxis().GetBinLowEdge(1))
						max_x_values.append(plotData.plotdict['root_objects'][nick].GetXaxis().GetBinUpEdge(plotData.plotdict['root_objects'][nick].GetNbinsX()))
					elif hasattr(plotData.plotdict['root_objects'][nick], "GetN") and hasattr(plotData.plotdict['root_objects'][nick], "GetX"):  # for TGraphs
						min_x_values.append(ROOT.TMath.MinElement(plotData.plotdict['root_objects'][nick].GetN(), plotData.plotdict['root_objects'][nick].GetX()))
						max_x_values.append(ROOT.TMath.MaxElement(plotData.plotdict['root_objects'][nick].GetN(), plotData.plotdict['root_objects'][nick].GetX()))
					else:
						# warn, but not for functions (from fitting)
						if not (all([len(vals)>0 for vals in [max_x_values, min_x_values]]) and isinstance(plotData.plotdict['root_objects'][nick], ROOT.TF1)):
							log.warning("PlotMpl: modify_axes(): Could not determine x-limits for nick '{0}'".format(nick))
				if min_x_values != [] and max_x_values != []:
					ax.set_xlim(min(min_x_values), max(max_x_values))
				else:
					log.warning("PlotMpl: modify_axes(): Could not automatically set x-limits!")
			if plotData.plotdict["y_lims"] is not None:
				ax.set_ylim(plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1])
			else:
				#if ax.dataLim.min[1] >= (-1E-6) and ax.get_ylim()[0] < 0.:
				main_root_objects = [plotData.plotdict["root_objects"][nick] for nick in plotData.plotdict["nicks"] if nick not in plotData.plotdict["subplot_nicks"]]
				if plotData.plotdict["y_log"]:
					if self.y_max > 1 and self.y_min < 1:
						ax.set_ylim(ymin=1)
					else:
						ax.set_ylim(ymin=self.y_min)
				else:
					if any("TH1" in obj.__class__.__name__ for obj in main_root_objects):
						ax.set_ylim(0)
					elif any("TH2" in obj.__class__.__name__ for obj in main_root_objects):
						ax.set_ylim(self.y_min, self.y_max)
					else:
						ax.set_ylim(self.y_min)
				if all("TH1" in obj.__class__.__name__ for obj in main_root_objects):
					ax.set_ylim(ymax=self.y_max * (plotData.plotdict["y_rel_lims"][1]))

		# set log scale
		for axis in ['x', 'y']:
			if plotData.plotdict[axis+"_log"]:
				getattr(ax, 'set_{}scale'.format(axis))(**{'value': 'log', 'nonpos'+axis: 'clip'})
				# for log plots, we have to set the ticks again and set the formatter:
				if plotData.plotdict[axis+'_ticks'] is not None:
					getattr(ax, 'set_{}ticks'.format(axis))(plotData.plotdict[axis+'_ticks'])
					getattr(ax, 'get_{}axis'.format(axis))().set_major_formatter(matplotlib.ticker.ScalarFormatter())

		for y, linestyle in zip(plotData.plotdict["lines"], self.default_linestyles*len(plotData.plotdict["lines"])):
			ax.axhline(y, color='black', linestyle=linestyle)

		# do special things for subplots
		if plotData.plotdict["subplot_nicks"]:
			ax2 = plotData.plot.axes[1]

			# Horizontal line at unity
			for y, linestyle in zip(plotData.plotdict["subplot_lines"], self.default_linestyles*len(plotData.plotdict["subplot_lines"])):
				ax2.axhline(y, color='black', linestyle=linestyle)

			if plotData.plotdict["y_subplot_lims"] != None:
				ax2.set_ylim(*plotData.plotdict["y_subplot_lims"])
			if plotData.plotdict["y_subplot_ticks"] is not None:
				ax2.set_yticks(plotData.plotdict["y_subplot_ticks"])

			ax2.set_xlabel(self.nicelabels.get_nice_label(plotData.plotdict["x_label"]),position=(1., 0.), va='top', ha='right')
			ax2.set_ylabel(plotData.plotdict["y_subplot_label"])
			if plotData.plotdict["subplot_grid"]:
				{'horizontal': ax2.yaxis, 'vertical':ax2.xaxis}.get(plotData.plotdict["subplot_grid"], ax2).grid(True)

			# Don't show ticklabels on main plot
			ax.xaxis.set_ticklabels([])
			if plotData.plotdict["x_tick_labels"] is not None:
				ax2.xaxis.set_ticklabels(plotData.plotdict["x_tick_labels"])
			# Ratio plot shares xlims of main plot
			ax2.set_xlim(ax.get_xlim())
			if plotData.plotdict["x_log"]:
				ax2.set_xscale('log', nonposx='clip')
			if plotData.plotdict["x_ticks"] is not None:
				ax2.set_xticks(plotData.plotdict["x_ticks"])
				if plotData.plotdict["x_log"]:
					ax2.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
			#ticklabels
			if plotData.plotdict["x_tick_labels"] is not None:
				if (not all([s.isdigit() for s in plotData.plotdict["x_tick_labels"]])):
					ax2.set_xticklabels(plotData.plotdict["x_tick_labels"], va="baseline")
					ax2.get_xaxis().set_tick_params(pad=16) # extra padding if ticklabels arent numbers
				else:
					ax2.set_xticklabels(plotData.plotdict["x_tick_labels"])

		# do special things for 2D Plots
		if self.plot_dimension == 2:
			if not plotData.plotdict["subplot_nicks"]:
				cb = plotData.plot.fig.colorbar(self.image, ax=ax)
				if plotData.plotdict["rasterized"]:
					cb.solids.set_rasterized(True)
			if plotData.plotdict["z_label"]:
				cb.set_label(self.nicelabels.get_nice_label(plotData.plotdict["z_label"]))
		elif self.plot_dimension == 3:
			if plotData.plotdict["z_label"]:
				ax.set_zlabel(self.nicelabels.get_nice_label(plotData.plotdict["z_label"]))


	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)
		# apparently MPL 121 has a bug for the PDF backend for legends without frame.
		# workaround: set legend.frameon True, but after legend creation set edgecolor to white.
		if ('pdf' in plotData.plotdict["formats"]) and (not matplotlib.rcParams['legend.frameon']) and (self.mpl_version == 121):
			matplotlib.rcParams['legend.frameon'] = True
			legend_workaround=True
		else:
			legend_workaround=False

		#iterate over all axis objects
		for ax in [plotData.plot.axes[0]]:

			if plotData.plotdict["title"]:
				ax.set_title(plotData.plotdict["title"], fontsize=18, horizontalalignment=('center' if (plotData.plotdict["energies"] is None) else 'right'))

			# Only plot legend if there active legend handles
			if len(ax.get_legend_handles_labels()[0]) > 1 and plotData.plotdict["legend"] is not None:
				legend = ax.legend(*self.get_legend_handles_labels_ordered(ax, plotData.plotdict['labels']), loc=plotData.plotdict["legend"], ncol=plotData.plotdict["legend_cols"], columnspacing=0.5, handletextpad=0.3)
				legend.set_zorder(100)
				if legend_workaround:
					legend.get_frame().set_edgecolor('#FFFFFF')

			if self.mpl_version >= 121:
				plt.tight_layout()
			# Decrease vertical distance between subplots
			if self.plot_dimension < 2:
				plotData.plot.fig.subplots_adjust(hspace=0.2)

		if len(plotData.plot.axes) > 1:
			for ax in [plotData.plot.axes[1]]:
				if len(ax.get_legend_handles_labels()[0]) > 1 and plotData.plotdict["subplot_legend"] is not None:
					legend = ax.legend(*self.get_legend_handles_labels_ordered(ax, plotData.plotdict['labels']), loc=plotData.plotdict["subplot_legend"], ncol=1, columnspacing=0.5, handletextpad=0.3)
					if legend_workaround:
						legend.get_frame().set_edgecolor('#FFFFFF')


	def add_texts(self, plotData):
		super(PlotMpl, self).add_texts(plotData)
		
		for ax in [plotData.plot.axes[0]]:
			if (not self.plot_dimension == 3) and (self.dataset_title != ""):
				plt.text(
						1.0, 1.030, self.dataset_title, zorder=100,
						transform=ax.transAxes, fontsize=18, horizontalalignment="right"
				)
		
		if plotData.plotdict["texts"] != [None]:
			for ax in [plotData.plot.axes[0]]:
				for x, y, text, size in zip(plotData.plotdict['texts_x'], plotData.plotdict['texts_y'], plotData.plotdict['texts'], plotData.plotdict["texts_size"]):
					ax.text(x, y, text, transform=ax.transAxes, fontsize=size, ha="left", va="top", zorder=100)

	def set_matplotlib_defaults(self):
		# Set matplotlib rc settings
		# matplotlib.rcParams['font.family'] = 'sans-serif'
		matplotlib.rcParams['mathtext.fontset'] = 'stixsans'
		matplotlib.rcParams['mathtext.default'] = 'rm'
		# matplotlib.rcParams['font.sans-serif'] = 'helvetica, Helvetica, Nimbus Sans L, Mukti Narrow, FreeSans'

		# figure
		matplotlib.rcParams['figure.figsize'] = 7., 7.
		# axes
		matplotlib.rcParams['axes.linewidth'] = 1
		matplotlib.rcParams['axes.labelsize'] = 20
		matplotlib.rcParams['xtick.major.pad'] = 7
		matplotlib.rcParams['ytick.major.pad'] = 7
		matplotlib.rcParams['xtick.labelsize'] = 16
		matplotlib.rcParams['xtick.major.size'] = 8
		if self.mpl_version >= 121:
			matplotlib.rcParams['xtick.major.width'] = 1.5
		matplotlib.rcParams['xtick.minor.size'] = 6
		if self.mpl_version >= 121:
			matplotlib.rcParams['xtick.minor.width'] = 1.
		matplotlib.rcParams['ytick.labelsize'] = 16
		if self.mpl_version >= 121:
			matplotlib.rcParams['ytick.major.width'] = 1.5
		matplotlib.rcParams['ytick.major.size'] = 8
		matplotlib.rcParams['ytick.minor.size'] = 6
		if self.mpl_version >= 121:
			matplotlib.rcParams['ytick.minor.width'] = 1.
		matplotlib.rcParams['lines.markersize'] = 8
		# default color cycle
		matplotlib.rcParams['axes.color_cycle'] = self.default_bar_colors
		matplotlib.rcParams["axes.formatter.limits"] = [-5, 5]
		# legend
		matplotlib.rcParams['legend.numpoints'] = 1
		matplotlib.rcParams['legend.fontsize'] = 16
		matplotlib.rcParams['legend.labelspacing'] = 0.3
		#matplotlib.rcParams['legend.frameon'] = False
		# Saving
		#matplotlib.rcParams['savefig.bbox'] = 'tight'
		matplotlib.rcParams['savefig.dpi'] = 150
		#matplotlib.rcParams['savefig.format'] = 'png'
		matplotlib.rcParams['agg.path.chunksize'] = 20000
		matplotlib.rcParams['pgf.texsystem'] = 'pdflatex'

	@staticmethod
	def steppify_bin(arr, isx=False):
		"""
		Produce stepped array of arr, needed for example for stepped fill_betweens.
		Pass all x bin edges to produce stepped x arr and all y bincontents to produce
		stepped bincontents representation
		steppify_bin([1,2,3], True) 
		-> [1,2,2,3]
		steppify_bin([5,6])
		-> [5,5,6,6]
		"""
		if isx:
			newarr = np.array(zip(arr[:-1], arr[1:])).ravel()
		else:
			newarr = np.array(zip(arr, arr)).ravel()
		return newarr

	def plot_errorbar(self, hist, step=False, show_xerr=True, show_yerr=True, emptybins=True, ax=None, **kwargs):
		""" Produce an errorbar plots with or without connecting lines.
		
		Args:
		    hist: MplHisto representation of a root histogram.
		    ax: Axis to plot on. If not specified current global axis will be used.
		    show_xerr: If True, x errorbars will be plotted.
		    show_yerr: If True, y errorbars will be plotted.
		    emptybins: Not Implemented. Supposed to ignore/plot empty bins.
		"""
	
		# if no axis passed use current global axis
		if ax is None:
			ax = plt.gca()
		x = hist.x
		if isinstance(hist, MplHisto):
			y = hist.bincontents
		else:
			y = hist.y

		if show_xerr:
			xerr = np.array((hist.xerrl, hist.xerru))
		else:
			xerr = None
		if show_yerr:
			if isinstance(hist, MplHisto):
				yerr = np.array((hist.binerrl, hist.binerru))
			else:
				yerr = np.array((hist.yerrl, hist.yerru))
		else:
			yerr = None

		line_style = kwargs.pop('linestyle', '')
		fillstyle = kwargs.pop('marker_fill_style', 'full')
		capsize = kwargs.pop('capsize', 0)
		fmt = kwargs.pop('fmt', '')
		if fmt in ['bar', 'fill']:
			log.warning('Invalid marker style. Default to \'.\'')
			fmt = '.'
		label = kwargs.pop('label', '')
		# Due to a bug in matplotlib v1.1 errorbar does not always respect line style when fmt is passed.
		# Workaround by plotting line and errorbars separately.
		# http://stackoverflow.com/a/18499120/3243729
		if line_style:
			if step:
				ax.step(self.steppify_bin(hist.xbinedges, isx=True), self.steppify_bin(y), fillstyle=fillstyle, linestyle=line_style, label=label, **kwargs)
			else:
				ax.plot(x, y, linestyle=line_style, fillstyle=fillstyle, label=label, **kwargs)
		ax.errorbar(x, y, fillstyle=fillstyle, xerr=xerr, yerr=yerr, capsize=capsize, fmt=fmt, label=label, linestyle='None', **kwargs)


	def plot_hist1d(self, hist, style='fill', ax=None, show_xerr=False, show_yerr=False, alpha=1.0, **kwargs):
		""" Plot a one-dimensional histogram.
		
		Args:
		    hist: MplHisto representation of a root histogram.
		    ax: Axis to plot on. If not specified current global axis will be used.
		    style: Mpl function to draw the bars, possible options are ['fill', 'bar']
		    show_xerr: If True, x errorbars will be plotted.
		    show_yerr: If True, y errorbars will be plotted.
		"""

		if ax is None:
			ax = plt.gca()

		color = kwargs.pop('color')
		edgecolor = kwargs.pop('edgecolor') if kwargs['edgecolor'] else color
		bottom = kwargs.pop('bottom', 0.)
		label = kwargs.pop('label', '')

		if style == 'fill':
			ax.fill_between(self.steppify_bin(hist.xbinedges, isx=True), self.steppify_bin(hist.bincontents), 
			                y2=bottom, color=color, edgecolor=edgecolor, alpha=alpha, zorder=1, label=label)
			# draw the legend proxy
			artist = plt.Rectangle((0, 0), 0, 0, label=label, facecolor=color, edgecolor=edgecolor, alpha=alpha)
			ax.add_patch(artist)
		elif style == 'bar':
			ax.bar(hist.xl, hist.bincontents, hist.xbinwidth, bottom=bottom,
			                label=label, fill=True, facecolor=color, edgecolor=edgecolor, ecolor=color, alpha=alpha)

		if show_xerr:
			xerr = np.array((hist.xerrl, hist.xerru))
		else:
			xerr = None
		if show_yerr:
			if isinstance(hist, MplHisto):
				yerr = np.array((hist.binerrl, hist.binerru))
			else:
				yerr = np.array((hist.yerrl, hist.yerru))
		else:
			yerr = None

		if show_xerr or show_yerr:
			ax.errorbar(hist.x, hist.bincontents, yerr=yerr, xerr=xerr, color=color, fmt='', capsize=0, zorder=1, linestyle='', label=label)


	def plot_contour1d(self, hist, ax=None, z_log=False, vmin=None, vmax=None, cmap='afmhot', rasterized=False):
		"""One dimensional contour plot.

		Args:
		    hist: MplHisto representation of a root histogram.
		    ax: Axis to plot on. If not specified current global axis will be used.
		    z_log: If True, z axis will be logarithmic.
		    vmin: Lower limit of z axis
		    vmax: Upper limit of z axis
		    cmap: Colormap used to
		"""
		if ax is None:
			ax = plt.gca()
		if (vmin, vmax) == (None,)*2:
			if z_log:
				vmin, vmax = np.min(hist.bincontents[np.nonzero(hist.bincontents)]), np.amax(hist.bincontents)
			else:
				vmin, vmax = np.amin(hist.bincontents), np.amax(hist.bincontents)
		norm = (LogNorm if z_log else Normalize)(vmin=vmin, vmax=vmax)

		# special settings for masked arrays (TProfile2Ds):
		if type(hist.bincontents) == np.ma.core.MaskedArray:
			min_color, max_color = cmap(norm(vmin))[:3], cmap(norm(vmax))[:3]  # get min and max colors from colorbar as rgb-tuples

			# set color for masked entries depending on min and max color of colorbar
			mask_color = 'white'
			if any([all([i>0.95 for i in color]) for color in [min_color, max_color]]):  # check if white is min or max color
				mask_color = 'black'
				if any([all([i<0.05 for i in color]) for color in [min_color, max_color]]):  # check if black is min or max color
					mask_color = 'red'
			cmap.set_bad(mask_color, alpha=None)
		return ax.pcolormesh(hist.xbinedges, hist.ybinedges, hist.bincontents, cmap=cmap, norm=norm, rasterized=rasterized)


	def plot_3d(self, hist, ax, cmap='afmhot', angle=0):
		""" Three-dimensional plot. 
		"""
		ax.view_init(20, angle-120)
		
		y = np.linspace(hist.y[0], hist.y[-1], len(hist.y))
		x = np.linspace(hist.x[0], hist.x[-1], len(hist.x))
		X, Y = np.meshgrid(x, y)

		ax.plot_surface(X, Y, hist.bincontents, rstride=1, cstride=1,
			cmap=cmap, linewidth=0, antialiased=True, shade=False)


	def plot_tgrapherrors_envelope(self, mplhist, ax, label, color, line_style, line_width, step, zorder, alpha):
		""" Plot the envelope given by y-errors around a TGraphErrors"""
		if step:
			# central values:
			ax.step(self.steppify_bin(mplhist.xbinedges, isx=True), self.steppify_bin(mplhist.y),
				label=label, linestyle=line_style, zorder=zorder)
			# plot errors as shaded area:
			ax.fill_between(
				self.steppify_bin(mplhist.xbinedges, isx=True),
				self.steppify_bin([(y_val+error) for y_val, error in zip(self.mplhist.y, self.mplhist.yerr)]),
				y2=self.steppify_bin([(y_val-error) for y_val, error in zip(self.mplhist.y, self.mplhist.yerr)]),
				color=color,
				label=label,
				alpha=alpha,
				zorder=zorder
			)
		else:
			# draw a smooth curve with error band around
			ax.plot(self.mplhist.x, self.mplhist.y, color=color, linestyle=line_style,
			        linewidth=line_width, zorder=zorder,label=label)
			ax.fill_between(self.mplhist.x,
				[(y_val-error) for y_val, error in zip(self.mplhist.y, self.mplhist.yerrl)],
				[(y_val+error) for y_val, error in zip(self.mplhist.y, self.mplhist.yerru)],
				facecolor=color,
				edgecolor=color,
				interpolate=False,
				alpha=alpha,
				zorder=zorder,
				label=label,
			)
			patch_for_label = plt.Rectangle((0, 0), 0, 0, label=label, color=color, alpha=alpha)
			ax.add_patch(patch_for_label)


	def get_zip_arguments(self, plotData):
		zip_arguments = (list(plotData.plotdict["nicks"]),
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["edgecolors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"],
		                                 plotData.plotdict["marker_fill_styles"],
		                                 plotData.plotdict["x_errors"],
		                                 plotData.plotdict["y_errors"],
		                                 plotData.plotdict["line_styles"],
		                                 plotData.plotdict["line_widths"],
		                                 plotData.plotdict["step"],
		                                 plotData.plotdict["zorder"],
		                                 plotData.plotdict["alphas"])
		return zip_arguments

	@staticmethod
	def get_legend_handles_labels_ordered(ax, labellist):
		handles, labels = ax.get_legend_handles_labels()
		if len(handles) > 0 and len(labels) > 0:
		# iterate over labels and combine handles if labels appears several times
		# this is necessary if we have multiple handles for one nick, e.g. if errorbar and plot/step is drawn separately
			new_labels = []
			new_handles = []
			for index, (label, handle) in enumerate(zip(labels, handles)):
				if label not in new_labels:  # if label not in new list, simply add
					new_labels.append(label)
					new_handles.append(handle)
				else:  # if label already in new list, combine handles to tuple
					if type(new_handles[new_labels.index(label)]) == tuple:
						new_handles[new_labels.index(label)] = (handles[index],) + new_handles[new_labels.index(label)]
					else:
						new_handles[new_labels.index(label)] = (new_handles[new_labels.index(label)], handles[index])
			# sort both labels and handles by order of 'labels' in plotdict
			labels, handles = zip(*sorted(zip(tuple(new_labels), tuple(new_handles)), key=lambda t: labellist.index(t[0])))
		return handles, labels

