# -*- coding: utf8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import AxesGrid

import Artus.HarryPlotter.tools.getroot as getroot
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.mplconvert as mplconvert
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.plot1d as plot1d


# TODO: split into ROOT and MPL version
def plot_2d(plotdict):
	""" """
	# if no plotdict are given, create:
	log.info("A 2D %s plot is created" % (plotdict['plot']))

	plot1d.get_root_histos(plotdict)
	plot1d.get_mpl_histos(plotdict)


	if len(plotdict['z']) == 0:
		# normalize to the same number of events
		if len(plotdict['mplhistos']) > 1 and plotdict['normalize']:
			pass #TODO implement proper normalization
		z_name = 'Events'
	else:
		if plotdict['zname'] is not None:
			z_name = plotdict['zname']
		else:
			z_name = plotdict['z'][0]
	if plotdict['zlims'] is None:
		plotdict['zlims'] = [0, np.max(plotdict['mplhistos'][0].BinContents)]

	#determine plot type: 2D Histogram or 2D Profile, and get the axis properties
	if plotdict['subplot'] == True:
		fig = fig_axes[0]
		grid = [fig_axes[1]]
	else:
		# create figure  + axes
		fig = plt.figure(figsize=(10. * len(plotdict['files']), 7.))
		plotdict['figure'] = fig
		grid = AxesGrid(fig, 111,
						nrows_ncols=(1, len(plotdict['files'])),
						axes_pad=0.4,
						share_all=True,
						aspect=False,
						label_mode="L",
						cbar_pad=0.2,
						cbar_location="right",
						cbar_mode='single',
						)

	for plot, label, ax in zip(plotdict['mplhistos'], plotdict['labels'], grid):
		ax.set_title(label)
		cmap1 = matplotlib.cm.get_cmap('jet')  # Blues
		ctext = 'white'
		if plotdict['fit'] and "profile" in plotdict['fit']:
			cmap1 = matplotlib.cm.get_cmap('Blues')  # Blues
			ctext = 'black'
		if "zlog" in plotdict and plotdict["zlog"]:
			thenorm = matplotlib.colors.LogNorm()
			vmin = max(plotdict['zlims'][0], 1)
		else:
			thenorm = None
			vmin = plotdict['zlims'][0]
		image = ax.imshow(plot.BinContents,
			interpolation='nearest',
			cmap=cmap1,
			origin='lower',
			aspect='auto',
			extent=[plot.xborderlow, plot.xborderhigh, plot.yborderlow, plot.yborderhigh],
			vmin=vmin,
			vmax=plotdict['zlims'][1],
			norm=thenorm)

		# profiles in plot:
		if plotdict['fit'] and "profile" in plotdict['fit']:
			cprofile = ['green', 'red']
			xstr = xydict.get(plotdict['xynames'][0], [plotdict['xynames'][0], ""])[0]
			ystr = xydict.get(plotdict['xynames'][1], [plotdict['xynames'][1], ""])[0]
			profx = getroot.root2histo(rootobjects[datamc.index(plot)].ProfileX())
			profy = getroot.root2histo(rootobjects[datamc.index(plot)].ProfileY())
			ax.errorbar(profx.xc, profx.y, profx.yerr, fmt='v', label=r"Profile $%s$" % xstr, color=cprofile[0])
			ax.errorbar(profy.y, profy.xc, xerr=profy.yerr, fmt='<', label=r"Profile $%s$" % ystr, color=cprofile[1])

		
		# labels:
		plotdict['axes'] = ax
		labels.add_labels(plotdict)
		utils.setaxislimits(plotdict)
		"""
		if plotdict['fit'] and "correlation" in plotdict['fit']:
			ax.text(0.04, 0.92, r"correlation: ${0:.3f}$".format(
				rootobjects[datamc.index(plot)].GetCorrelationFactor()),
				color=ctext, transform=ax.transAxes)
		if plotdict['fit'] and "average" in plotdict['fit']:
			ax.text(0.04, 0.85, r"$\langle {0:s} \rangle = {2:.1f} \,\mathrm{{ {1:s} }}$".format(
				xydict.get(plotdict['xynames'][0], [plotdict['xynames'][0], ""])[0],
				xydict.get(plotdict['xynames'][0], [plotdict['xynames'][0], ""])[1],
				rootobjects[datamc.index(plot)].GetMean(1)),
				color=ctext, transform=ax.transAxes)
			ax.text(0.04, 0.80, r"$\langle {0:s} \rangle = {2:.1f} \,\mathrm{{ {1:s} }}$".format(
				xydict.get(plotdict['xynames'][1], [plotdict['xynames'][1], ""])[0],
				xydict.get(plotdict['xynames'][1], [plotdict['xynames'][1], ""])[1],
				rootobjects[datamc.index(plot)].GetMean(2)),
				color=ctext, transform=ax.transAxes)
		"""


	#add the colorbar
	cb = fig.colorbar(image, cax=grid.cbar_axes[0], ax=ax)
	cb.set_label(z_name)

	utils.save(plotdict)
