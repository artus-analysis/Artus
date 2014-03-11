#!/usr/bin/env python
# -*- coding: utf8 -*-

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1 import AxesGrid

import Artus.HarryPlotter.tools.getroot as getroot
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.mplconvert as mplconvert
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.plot1d as plot1d


def plot_2d(plotdict):
	""" """
	# if no plotdict are given, create:
	print "A 2D %s plot is created" % (plotdict['plot'])

	plot1d.get_root_histos(plotdict)



	if len(quantity.split("_")) == 2:
		# normalize to the same number of events
		if len(datamc) > 1 and plotdict['normalize']:
			for d in datamc[1:]:
				if d.binsum() > 0.0 and datamc[0].binsum() > 0:
					d.scale(datamc[0].binsum() / d.binsum())
		z_name = 'Events'
		if plotdict['z'] is None:
			plotdict['z'] = [0, np.max(datamc[0].BinContents)]
	else:
		if plotdict['z'] is None:
			plotdict['z'] = utils.getaxislabels_list(quantity.split("_")[0])[:2]
		if plotdict['xynames'] is not None and len(plotdict['xynames']) > 2:
			z_name = plotdict['xynames'][2]
		else:
			z_name = quantity.split("_")[0]

	#determine plot type: 2D Histogram or 2D Profile, and get the axis properties
	if plotdict['subplot'] == True:
		fig = fig_axes[0]
		grid = [fig_axes[1]]
	else:
		# create figure  + axes
		fig = plt.figure(figsize=(10. * len(plotdict['files']), 7.))
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
		print plotdict['z']
		if "zlog" in plotdict and plotdict["zlog"]:  # TODO
			thenorm = matplotlib.colors.LogNorm()
			plotdict['z'][0] = min(plotdict['z'], 1)
		else:
			thenorm = None
		image = ax.imshow(plot.BinContents,
			interpolation='nearest',
			cmap=cmap1,
			origin='lower',
			aspect='auto',
			extent=[plot.xborderlow, plot.xborderhigh, plot.yborderlow, plot.yborderhigh],
			vmin=plotdict['z'][0],
			vmax=plotdict['z'][1],)  # norm=matplotlib.colors.LogNorm())

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
		labels.axislabels(ax, plotdict['xynames'][0], plotdict['xynames'][1],
															plotdict=plotdict)
		labels.labels(ax, opt, plotdict, plotdict['subplot'])
		utils.setaxislimits(ax, plotdict)
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


	#add the colorbar
	cb = fig.colorbar(image, cax=grid.cbar_axes[0], ax=ax)
	cb.set_label(labels.unitformat(labels.getaxislabels_list(z_name)[2],
								labels.getaxislabels_list(z_name)[3], False))

	utils.Save(plotdict)
