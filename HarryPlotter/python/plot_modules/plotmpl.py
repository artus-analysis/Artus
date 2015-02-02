#  -*- coding: utf-8 -*-

import os
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.plotbase as plotbase

from Artus.HarryPlotter.utility.mplhisto import MplHisto, MplGraph
import Artus.HarryPlotter.utility.labels as labels

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize

from itertools import cycle

import numpy as np

class PlotMpl(plotbase.PlotBase):
	"""Create scientific plots using the Matplotlib backend."""

	def __init__(self):
		super(PlotMpl, self).__init__()
		self.mpl_version = int(matplotlib.__version__.replace(".", ""))
		self.set_matplotlib_defaults()
		self.nicelabels = labels.LabelsDict()

	def modify_argument_parser(self, parser, args):
		super(PlotMpl, self).modify_argument_parser(parser, args)
		self.formatting_options.add_argument("--colormap", default="afmhot", nargs="?",
		                                     help="Colormap for matplotlib [Default: '%(default)s']")
		self.formatting_options.add_argument("--edgecolors", nargs="+",
		                                     help="Edgecolor to be passed to plot objects.")
		self.formatting_options.add_argument("--step", default=False, type='bool', nargs="+",
		                                     help="Step lines according to bin edges instead of connecting points.")
		self.formatting_options.add_argument("--zorder", type=int, nargs="+",
		                                     help="Order in which plots are layered. Default is first nick always on top, then zorder increases.")

	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)

		self.prepare_list_args(plotData, ["nicks", "step", "zorder", "edgecolors"])
		
		# Set default values for colors if not provided
		default_colorcycle = cycle(matplotlib.rcParams['axes.color_cycle'])
		for index, color in enumerate(plotData.plotdict["colors"]):
			if color is None:
				plotData.plotdict["colors"][index] = next(default_colorcycle)
			else:
				plotData.plotdict["colors"][index] = color
		# for index, edgecolor in enumerate(plotData.plotdict["edgecolors"]):
		# 	if edgecolor is None:
		# 		plotData.plotdict["edgecolors"][index] = 'black'

		self.set_default_ratio_colors(plotData)

		if plotData.plotdict["legloc"] is None:
			plotData.plotdict["legloc"] = "upper right"
		# defaults for markers
		for index, marker in enumerate(plotData.plotdict["markers"]):
			if marker is None:
				if index == 0 and len(plotData.plotdict["markers"]) > 1 :
					plotData.plotdict["markers"][index] = "."
				else:
					plotData.plotdict["markers"][index] = "fill"

		for index, marker in enumerate(plotData.plotdict["zorder"]):
			if marker is None:
				if index == 0:
					plotData.plotdict["zorder"][index] = 99
				else:
					plotData.plotdict["zorder"][index] = index

		for index, marker in enumerate(plotData.plotdict["ratio_markers"]):
			if marker is None:
					plotData.plotdict["ratio_markers"][index] = '.'

		# default for linestyles
		for index, linestyle in enumerate(plotData.plotdict["linestyles"]):
			if linestyle is None:
				plotData.plotdict["linestyles"][index] = ""

		# validate length of parameters first
		zip_arguments = self.get_zip_arguments(plotData)
		for argument in zip_arguments:
			if len(argument) != len(zip_arguments[0]):
				log.warning("The PlotMpl module is trying to make plots with invalid inputs. The Plot will eventually not contain all requested information.")
				log.warning("This the invalid input: %s" % argument)
				break

	def run(self, plotData):
		super(PlotMpl, self).run(plotData)

	def set_style(self, plotData):
		super(PlotMpl, self).set_style(plotData)
	
	def create_canvas(self, plotData):
		self.fig = plt.figure(figsize = [
			plotData.plotdict['n_axes_x'] * matplotlib.rcParams['figure.figsize'][0],
			plotData.plotdict['n_axes_y'] * matplotlib.rcParams['figure.figsize'][1]
			])
		if plotData.plotdict["ratio"]:
			self.axes = [plt.subplot2grid((4,1), (0, 0), rowspan=3)]
			self.ax2 = plt.subplot2grid((4,1), (3, 0))
		else:
			self.axes = []
			for i in range(plotData.plotdict['n_axes_x'] * plotData.plotdict['n_axes_y']):
				self.axes += [self.fig.add_subplot(plotData.plotdict['n_axes_x'], plotData.plotdict['n_axes_y'], i)]

		if plotData.plotdict["ratio"] and plotData.plotdict["y_ratio_lims"] != None:
			self.ax2.set_ylim(plotData.plotdict["y_ratio_lims"][0],plotData.plotdict["y_ratio_lims"][1])

	def prepare_histograms(self, plotData):
		super(PlotMpl, self).prepare_histograms(plotData)


	def make_plots(self, plotData):
		zip_arguments = self.get_zip_arguments(plotData)

		for nick, color, edgecolor, label, marker, x_error, y_error, linestyle, step, zorder, n_ax in zip(*zip_arguments):
			log.info("Process nick: {0}".format(nick))
			root_object = plotData.plotdict["root_objects"][nick]

			if isinstance(root_object, ROOT.TGraph):
				self.plot_dimension = 1
				mplhist = MplGraph(root_object)
				self.plot_errorbar(mplhist, ax=self.axes[n_ax],
				                   show_xerr=x_error, show_yerr=y_error,
				                   color=color, fmt=marker, capsize=0,
				                   linestyle=linestyle, label=label, zorder=zorder)

			elif isinstance(root_object, ROOT.TH2):
				mplhist = MplHisto(root_object)
				self.plot_dimension = mplhist.dimension
				vmin = plotData.plotdict["z_lims"][0] if plotData.plotdict["z_lims"] else None
				vmax = plotData.plotdict["z_lims"][1] if plotData.plotdict["z_lims"] else None
				cmap = plt.cm.get_cmap(plotData.plotdict["colormap"])
				self.image = self.plot_contour1d(mplhist, ax=self.axes[n_ax], vmin=vmin, vmax=vmax, z_log=plotData.plotdict["z_log"], cmap=cmap)

			elif isinstance(root_object, ROOT.TH1):
				mplhist = MplHisto(root_object)
				self.plot_dimension = mplhist.dimension

				if marker=="bar":
					self.plot_hist1d(mplhist, style='bar', ax=self.axes[n_ax], show_yerr=y_error, label=label, color=color, edgecolor=edgecolor, alpha=1.0, zorder=zorder)
				elif marker=='fill':
					self.plot_hist1d(mplhist, style='fill', ax=self.axes[n_ax], show_yerr=y_error, label=label, color=color, edgecolor=edgecolor, alpha=1.0, zorder=zorder)
				else:
					self.plot_errorbar(mplhist, ax=self.axes[n_ax],
					                   show_xerr=x_error, show_yerr=y_error,
					                   step=step, color=color, fmt=marker,
					                   capsize=2, linestyle=linestyle, label=label, zorder=zorder)

			# draw functions from dictionary
			elif isinstance(root_object, ROOT.TF1):
				x_values = []
				y_values = []
				sampling_points = 1000
				x_range = [ root_object.GetXaxis().GetXmin(), root_object.GetXaxis().GetXmax()] 
				for x in np.arange(x_range[0], x_range[1], (float(x_range[1])-float(x_range[0]))/sampling_points):
					x_values.append(x)
					y_values.append(root_object.Eval(x))
				self.axes[n_ax].plot(x_values, y_values, label=label, color=color, linestyle=linestyle, linewidth=2)

		if plotData.plotdict["ratio"]:
			for root_object, ratio_color, ratio_x_error, ratio_y_error, ratio_marker, in zip(plotData.plotdict["root_ratio_histos"],
				                                               plotData.plotdict["ratio_colors"],
				                                               plotData.plotdict["ratio_x_errors"],
				                                               plotData.plotdict["ratio_y_errors"],
				                                               plotData.plotdict["ratio_markers"]):
				self.ax2.axhline(1.0, color='black')
				mplhist_ratio = MplHisto(root_object)
				self.plot_errorbar(mplhist_ratio, ax=self.ax2,
				                   show_xerr=ratio_x_error, show_yerr=ratio_y_error,
				                   step=step, color=ratio_color, fmt=ratio_marker,
				                   capsize=0, linestyle=linestyle, zorder=zorder)

	def modify_axes(self, plotData):
		# do what is needed for all plots:
		super(PlotMpl, self).modify_axes(plotData)

		#iterate over all axis objects
		for ax in self.axes:

			ax.grid(plotData.plotdict["grid"])

			if not plotData.plotdict["ratio"]:
				ax.set_xlabel(self.nicelabels.get_nice_label(plotData.plotdict["x_label"]), position=(1., 0.), va='top', ha='right')
			ax.set_ylabel(self.nicelabels.get_nice_label(plotData.plotdict["y_label"]), position=(0., 1.), va='top', ha='right')
			ax.ticklabel_format(style='sci',scilimits=(-3,4),axis='both')

			if plotData.plotdict["x_lims"] is not None:
				ax.set_xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
			if plotData.plotdict["y_lims"] is not None:
				ax.set_ylim(plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1])
			else:
				if ax.dataLim.min[1] >= (-1E-6) and ax.get_ylim()[0] < 0.:
					ax.set_ylim(ymin=0.0)

			# do special things for 1D Plots
			if self.plot_dimension == 1:
				if plotData.plotdict["x_log"]: 
					ax.set_xscale('log', nonposx='clip')
				if plotData.plotdict["y_log"]: 
					ax.set_yscale('log', nonposy='clip')

				if plotData.plotdict["ratio"]:
					self.ax2.set_xlabel(self.nicelabels.get_nice_label(plotData.plotdict["x_label"]),position=(1., 0.), va='top', ha='right')
					self.ax2.set_ylabel(plotData.plotdict["y_ratio_label"])
					self.ax2.grid(plotData.plotdict["ratio_grid"])
					# Don't show ticklabels on main plot
					ax.xaxis.set_ticklabels([])
					# Ratio plot shares xlims of main plot
					self.ax2.set_xlim(ax.get_xlim())
					if plotData.plotdict["x_log"]: 
						self.ax2.set_xscale('log', nonposx='clip')
			# do special things for 2D Plots
			elif self.plot_dimension == 2:
				cb = self.fig.colorbar(self.image, ax=self.axes[n_ax])
				if plotData.plotdict["z_label"]:
					cb.set_label(plotData.plotdict["z_label"])

	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)

		#iterate over all axis objects
		for ax in self.axes:

			if plotData.plotdict["title"]:
				ax.set_title(plotData.plotdict["title"], fontsize=14)

			if not (plotData.plotdict["lumi"]==None):
				plt.text(-0.0, 1.030, "$\mathcal{L}=%s\mathrm{fb^{-1}}$" % plotData.plotdict["lumi"],
					transform=ax.transAxes, fontsize=18)
			if not (plotData.plotdict["energy"] == None):
				energy = "+".join(plotData.plotdict["energy"])
				plt.text(1.0, 1.030, "$\sqrt{s}=" + energy + "\\ \mathrm{TeV}$", 
					transform=ax.transAxes, fontsize=18, horizontalalignment="right")

			# Only plot legend if there active legend handles
			if ax.get_legend_handles_labels()[0]:
				# handles, labels = ax.get_legend_handles_labels()
				# sort both labels and handles by labels
				# labels, handles = zip(*sorted(zip(labels, handles), key=lambda t: t[0]))
				ax.legend(loc=plotData.plotdict["legloc"])

			if self.mpl_version >= 121:
				plt.tight_layout()
			# Decrease vertical distance between subplots
			plt.subplots_adjust(hspace=0.2)

	def save_canvas(self, plotData):
		for output_filename in plotData.plotdict["output_filenames"]:
			self.fig.savefig(output_filename)
			log.info("Created plot \"%s\"." % output_filename)

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
		matplotlib.rcParams['axes.color_cycle'] = [(0.0, 0.0, 0.0),
		                                           (0.21568627450980393, 0.49411764705882355, 0.7215686274509804),
		                                           (0.30196078431372547, 0.6862745098039216, 0.2901960784313726),
		                                           (0.596078431372549, 0.3058823529411765, 0.6392156862745098),
		                                           (1.0, 0.4980392156862745, 0.0),
		                                           (1.0, 1.0, 0.2),
		                                           (0.6509803921568628, 0.33725490196078434, 0.1568627450980392),
		                                           (0.9686274509803922, 0.5058823529411764, 0.7490196078431373),
		                                           (0.6, 0.6, 0.6)]
		matplotlib.rcParams["axes.formatter.limits"] = [-5, 5]
		# legend
		matplotlib.rcParams['legend.numpoints'] = 1
		matplotlib.rcParams['legend.fontsize'] = 19
		matplotlib.rcParams['legend.labelspacing'] = 0.3
		#matplotlib.rcParams['legend.frameon'] = False
		# Saving
		#matplotlib.rcParams['savefig.bbox'] = 'tight'
		matplotlib.rcParams['savefig.dpi'] = 150
		#matplotlib.rcParams['savefig.format'] = 'png'
		matplotlib.rcParams['agg.path.chunksize'] = 20000

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

		linestyle = kwargs.pop('linestyle', '')
		capsize = kwargs.pop('capsize', 0)
		fmt = kwargs.pop('fmt', '')
		if fmt in ['bar', 'fill']:
			log.warning('Invalid marker style. Default to \'.\'')
			fmt = '.'
		label = kwargs.pop('label', '')
		# Due to a bug in matplotlib v1.1 errorbar does not always respect linestyle when fmt is passed.
		# Workaround by plotting line and errorbars separately.
		# http://stackoverflow.com/a/18499120/3243729
		if linestyle:
			if step:
				ax.step(self.steppify_bin(hist.xbinedges, isx=True), self.steppify_bin(y), linestyle=linestyle, **kwargs)
			else:
				ax.plot(x, y, linestyle=linestyle, **kwargs)
		artist = ax.errorbar(x, y, xerr=xerr, yerr=yerr, label=label, capsize=capsize, fmt=fmt, linestyle='None', **kwargs)
		return artist

	def plot_hist1d(self, hist, style='fill', ax=None, show_xerr=False, show_yerr=False, **kwargs):
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
			                y2=bottom, color=color, edgecolor=edgecolor, alpha=1.0, zorder=1)
			# draw the legend proxy
			artist = plt.Rectangle((0, 0), 0, 0, label=label, facecolor=color, edgecolor=edgecolor, alpha=1.0)
			ax.add_patch(artist)
		elif style == 'bar':
			artist = ax.bar(hist.xl, hist.bincontents, hist.xbinwidth, bottom=bottom,
			                label=label, fill=True, facecolor=color, edgecolor=edgecolor, ecolor=color, alpha=1.0)

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
			ax.errorbar(hist.x, hist.bincontents, yerr=yerr, xerr=xerr, color=color, fmt='', capsize=0, zorder=1, linestyle='')

		return artist

	def plot_contour1d(self, hist, ax=None, z_log=False, vmin=None, vmax=None, cmap='afmhot'):
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

		arr = hist.bincontents
		if z_log:
			norm = LogNorm(vmin=vmin, vmax=vmax)
			arr = np.ma.masked_equal(arr, 0.0)
		else:
			norm = Normalize(vmin=vmin, vmax=vmax)

		# TODO Masked values are currently plotted black. Needs to be adapted to chosen colorbar
		cmap.set_bad('black', alpha=None)
		artist = ax.pcolormesh(hist.xbinedges, hist.ybinedges, arr, cmap=cmap, norm=norm)
		return artist

	def get_zip_arguments(self, plotData):
		zip_arguments = (list(plotData.plotdict["nicks"]),
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["edgecolors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"],
		                                 plotData.plotdict["x_errors"],
		                                 plotData.plotdict["y_errors"],
		                                 plotData.plotdict["linestyles"],
		                                 plotData.plotdict["step"],
		                                 plotData.plotdict["zorder"],
		                                 plotData.plotdict["axes"])
		return zip_arguments


