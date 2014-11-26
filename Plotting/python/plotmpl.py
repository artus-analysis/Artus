#  -*- coding: utf-8 -*-

"""
"""
import os
import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import HarryPlotter.Plotting.plotbase as plotbase

from HarryPlotter.Utility.mplhisto import MplHisto, MplGraph

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.colors import Normalize

import numpy as np

class PlotMpl(plotbase.PlotBase):
	def __init__(self):
		super(PlotMpl, self).__init__()
		self.mpl_version = int(matplotlib.__version__.replace(".", ""))
		self.set_matplotlib_defaults()

	def modify_argument_parser(self, parser, args):
		super(PlotMpl, self).modify_argument_parser(parser, args)
		self.formatting_options.add_argument("--colormap", default="afmhot", nargs="?",
		                                     help="Colormap for matplotlib [Default: 'afmhot']")
		self.formatting_options.add_argument("--step", default=False, type='bool', nargs="+",
		                                     help="Step lines according to bin edges instead of connecting points.")
		self.formatting_options.add_argument("--zorder", type=int, nargs="+",
		                                     help="Order in which plots are layered. Default is first nick always on top, then zorder increases.")

	def prepare_args(self, parser, plotData):
		super(PlotMpl, self).prepare_args(parser, plotData)

		self.prepare_list_args(plotData, ["step", "zorder"], len(plotData.plotdict["markers"]))
		# Set meaningful default values for colors if none provided
		default_colorcycle = iter(matplotlib.rcParams['axes.color_cycle'])
		for index, color in enumerate(plotData.plotdict["colors"]):
			if color is None:
				plotData.plotdict["colors"][index] = next(default_colorcycle)
			else:
				plotData.plotdict["colors"][index] = color
		self.set_default_ratio_colors(plotData)
		
		# defaults for markers
		for index, marker in enumerate(plotData.plotdict["markers"]):
			if marker is None:
				if index == 0:
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
				plotData.plotdict["linestyles"][index] = "-"

		# validate length of parameters first
		zip_arguments = self.get_zip_arguments(plotData)
		for argument in zip_arguments:
			if len(argument) != len(zip_arguments[0]):
				log.warning("The PlotMpl module is trying to make plots with invalid inputs. The Plot will eventually not contain all requested information.")
				break

	def run(self, plotData):
		super(PlotMpl, self).run(plotData)
	
	def create_canvas(self, plotData):
		self.fig = plt.figure()
		if plotData.plotdict["ratio"]:
			self.ax = plt.subplot2grid((4,1), (0, 0), rowspan=3)
			self.ax2 = plt.subplot2grid((4,1), (3, 0))
		else:
			self.ax = self.fig.add_subplot(111)


		if plotData.plotdict["x_lims"] is not None:
			self.ax.set_xlim([plotData.plotdict["x_lims"][0],plotData.plotdict["x_lims"][1]])
		if plotData.plotdict["y_lims"] is not None:
			self.ax.set_ylim(plotData.plotdict["y_lims"][0],plotData.plotdict["y_lims"][1])

		if plotData.plotdict["ratio"] and plotData.plotdict["y_ratio_lims"] != None:
			self.ax2.set_ylim(plotData.plotdict["y_ratio_lims"][0],plotData.plotdict["y_ratio_lims"][1])

	def prepare_histograms(self, plotData):
		self.bottom_y_values = {}


	def make_plots(self, plotData):
		zip_arguments = self.get_zip_arguments(plotData)

		for nick, color, label, marker, errorbar, linestyle, step, zorder in zip(*zip_arguments):
			log.info("Process nick: {0}".format(nick))
			root_object = plotData.plotdict["root_objects"][nick]

			if isinstance(root_object, ROOT.TGraph):
				self.plot_dimension = 1
				mplhist = MplGraph(root_object)
				self.plot_errorbar(mplhist, ax=self.ax, style='line', color=color, fmt=marker, capsize=0, linestyle=linestyle, label=label, zorder=zorder)
			elif isinstance(root_object, ROOT.TH2):
				mplhist = MplHisto(root_object)
				self.plot_dimension = mplhist.dimension
				vmin = plotData.plotdict["z_lims"][0] if plotData.plotdict["z_lims"] else None
				vmax = plotData.plotdict["z_lims"][1] if plotData.plotdict["z_lims"] else None
				cmap = plt.cm.get_cmap(plotData.plotdict["colormap"])
				self.plot_contour1d(mplhist, ax=self.ax, vmin=vmin, vmax=vmax, z_log=plotData.plotdict["z_log"], cmap=cmap)

			elif isinstance(root_object, ROOT.TH1):
				mplhist = MplHisto(root_object)
				self.plot_dimension = mplhist.dimension

				if marker=="bar":
					self.plot_hist1d(mplhist, style='bar', ax=self.ax, show_yerr=errorbar, label=label, color=color, alpha=1.0, zorder=zorder)
				elif marker=='fill':
					self.plot_hist1d(mplhist, style='fill', ax=self.ax, show_yerr=errorbar, label=label, color=color, alpha=1.0, zorder=zorder)
				else:
					self.plot_errorbar(mplhist, ax=self.ax, step=step, color=color, fmt=marker, capsize=0, linestyle=linestyle, label=label, zorder=zorder)
			# draw functions from dictionary
			elif isinstance(root_object, ROOT.TF1):
				x_values = []
				y_values = []
				sampling_points = 1000
				x_range = [ root_object.GetXaxis().GetXmin(), root_object.GetXaxis().GetXmax()] 
				for x in np.arange(x_range[0], x_range[1], (float(x_range[1])-float(x_range[0]))/sampling_points):
					x_values.append(x)
					y_values.append(root_object.Eval(x))
				self.ax.plot(x_values, y_values, label=label, color=color, linestyle=linestyle, linewidth=2)

		if plotData.plotdict["ratio"]:
			for root_object, ratio_color, ratio_marker, in zip(plotData.plotdict["root_ratio_histos"],
				                                               plotData.plotdict["ratio_colors"],
				                                               plotData.plotdict["ratio_markers"]):
				self.ax2.axhline(1.0, color='black')
				mplhist_ratio = MplHisto(root_object)
				# self.ax2.errorbar(mplhist_ratio.x, mplhist_ratio.y, mplhist_ratio.yerr, ecolor=ratio_color, fmt=ratio_marker)
				self.plot_errorbar(mplhist_ratio, ax=self.ax2, step=step, color=ratio_color, fmt=ratio_marker, capsize=0, linestyle=linestyle, zorder=zorder)



	def modify_axes(self, plotData):
		# do what is needed for all plots:
		super(PlotMpl, self).modify_axes(plotData)

		self.ax.grid(plotData.plotdict["grid"])

		if not plotData.plotdict["ratio"]:
			self.ax.set_xlabel(plotData.plotdict["x_label"], position=(1., 0.), va='top', ha='right')
		self.ax.set_ylabel(plotData.plotdict["y_label"], position=(0., 1.), va='top', ha='right')
		self.ax.ticklabel_format(style='sci',scilimits=(-3,4),axis='both')

		# do special things for 1D Plots
		if self.plot_dimension == 1:
			if plotData.plotdict["x_log"]: 
				self.ax.set_xscale('log', nonposx='clip')
			if plotData.plotdict["y_log"]: 
				self.ax.set_yscale('log', nonposy='clip')

			if plotData.plotdict["ratio"]:
				self.ax2.set_xlabel(plotData.plotdict["x_label"],position=(1., 0.), va='top', ha='right')
				self.ax2.set_ylabel(plotData.plotdict["y_ratio_label"])
				self.ax2.grid(plotData.plotdict["ratio_grid"])
				# Don't show ticklabels on main plot
				self.ax.xaxis.set_ticklabels([])
				# Ratio plot shares xlims of main plot
				self.ax2.set_xlim(self.ax.get_xlim())
				if plotData.plotdict["x_log"]: 
					self.ax2.set_xscale('log', nonposx='clip')
		# do special things for 2D Plots
		elif self.plot_dimension == 2:
			cb = self.fig.colorbar(self.image, ax=self.ax)
			if plotData.plotdict["z_label"]:
				cb.set_label(plotData.plotdict["z_label"])


	def add_labels(self, plotData):
		super(PlotMpl, self).add_labels(plotData)

		if plotData.plotdict["title"]:
			self.ax.set_title(plotData.plotdict["title"], fontsize=14)

		if not (plotData.plotdict["lumi"]==None):
			plt.text(-0.0, 1.030, "$\mathcal{L}=" + str(plotData.plotdict["lumi"]) + "\mathrm{fb^{-1}}$", transform=self.ax.transAxes, fontsize=10)
			#self.fig.suptitle("$\mathcal{L}=" + str(plotData.plotdict["lumi"]) + "\mathrm{fb^{-1}}$", ha="center", x=0.4)
		if not (plotData.plotdict["energy"] == None):
			energy = "+".join(plotData.plotdict["energy"])
			plt.text(1.0, 1.030, "$\sqrt{s}=" + energy + "\\ \mathrm{TeV}$", transform=self.ax.transAxes, fontsize=10, horizontalalignment="right")
			#self.fig.suptitle("$\sqrt{s}=" + energy + "\\ \mathrm{TeV}$", ha="right",x=0.9) 

		self.ax.legend(loc='upper right')

		if self.mpl_version >= 121:
			plt.tight_layout()
		# Decrease vertical distance between subplots
		plt.subplots_adjust(hspace=0.2)

	def save_canvas(self, plotData):
		for output_filename in plotData.plotdict["output_filenames"]:
			self.fig.savefig(output_filename)
			log.info("Created plot \"%s\"." % output_filename)

	@staticmethod
	def set_matplotlib_defaults():
		# Set matplotlib rc settings
		# matplotlib.rcParams['font.family'] = 'sans-serif'
		matplotlib.rcParams['mathtext.fontset'] = 'stixsans'
		matplotlib.rcParams['mathtext.default'] = 'rm'
		# matplotlib.rcParams['font.sans-serif'] = 'helvetica, Helvetica, Nimbus Sans L, Mukti Narrow, FreeSans'

		# figure
		matplotlib.rcParams['figure.figsize'] = 7., 7.
		# axes
		matplotlib.rcParams['axes.linewidth'] = 2
		matplotlib.rcParams['axes.labelsize'] = 20
		matplotlib.rcParams['xtick.labelsize'] = 16
		matplotlib.rcParams['xtick.major.size'] = 8
		#matplotlib.rcParams['xtick.major.width'] = 1.5
		matplotlib.rcParams['xtick.minor.size'] = 6
		#matplotlib.rcParams['xtick.minor.width'] = 1.
		matplotlib.rcParams['ytick.labelsize'] = 16
		#matplotlib.rcParams['ytick.major.width'] = 1.5
		matplotlib.rcParams['ytick.major.size'] = 8
		matplotlib.rcParams['ytick.minor.size'] = 6
		#matplotlib.rcParams['ytick.minor.width'] = 1.
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
		label = kwargs.pop('label', '')
		# Due to a bug in matplotlib v1.1 errorbar does not always respect linestyle when fmt is passed.
		# Workaround by plotting line and errorbars separately.
		# http://stackoverflow.com/a/18499120/3243729
		if linestyle:
			if step:
				ax.step(self.steppify_bin(hist.xbinedges, isx=True), self.steppify_bin(y), linestyle=linestyle, **kwargs)
			else:
				ax.plot(x, y, linestyle=linestyle, **kwargs)

		ax.errorbar(x, y, xerr=xerr, yerr=yerr, label=label, capsize=capsize, fmt=fmt, linestyle='None', **kwargs)

	def plot_hist1d(self, hist, style='fill', ax=None, show_yerr=None, **kwargs):

		if ax is None:
			ax = plt.gca()

		color = kwargs.pop('color')
		bottom = kwargs.pop('bottom', 0.)
		label = kwargs.pop('label', '')

		if style == 'fill':
			ax.fill_between(self.steppify_bin(hist.xbinedges, isx=True), self.steppify_bin(hist.bincontents), 
			                y2=bottom, color=color, alpha=1.0, zorder=1)
			# draw the legend proxy
			proxy = plt.Rectangle((0, 0), 0, 0, label=label, facecolor=color, edgecolor='black', alpha=1.0)
			ax.add_patch(proxy)
		elif style == 'bar':
			ax.bar(hist.xl, hist.bincontents, hist.xbinwidth, bottom=bottom,
			       label=label, fill=True, facecolor=color, edgecolor=color, ecolor=color, alpha=1.0)

		if show_yerr:
			ax.errorbar(hist.x, hist.bincontents, yerr=hist.binerr, color=color, fmt='', capsize=0, zorder=1, linestyle='')

	def plot_contour1d(self, hist, ax=None, z_log=False, vmin=None, vmax=None, cmap='afmhot'):

		arr = hist.bincontents
		if z_log:
			norm = LogNorm(vmin=vmin, vmax=vmax)
			arr = np.ma.masked_equal(arr, 0.0)
		else:
			norm = Normalize(vmin=vmin, vmax=vmax)

		# TODO Masked values are currently plotted black. Needs to be adapted to chosen colorbar
		cmap.set_bad('black')
		self.image = self.ax.imshow(arr,
		                            interpolation='nearest',
		                            origin='lower',
		                            extent=[hist.xl[0], hist.xu[-1], hist.yl[0], hist.yu[-1]],
		                            aspect='auto',
		                            cmap=cmap,
		                            norm=norm)

	def get_zip_arguments(self, plotData):
		zip_arguments = (list(plotData.plotdict["nicks"]),
		                                 plotData.plotdict["colors"],
		                                 plotData.plotdict["labels"],
		                                 plotData.plotdict["markers"],
		                                 plotData.plotdict["errorbars"],
		                                 plotData.plotdict["linestyles"],
		                                 plotData.plotdict["step"],
		                                 plotData.plotdict["zorder"])
		return zip_arguments


