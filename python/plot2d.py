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


# this should go to the tools/dictionaries
xydict = {
	'npu': [r"n_\mathrm{PU}", ""],
	'nputruth': [r"n_\mathrm{PU}^\mathrm{mean}", ""],
	'npv': [r"n_\mathrm{PV}", ""],
	'rho': [r"\rho", "GeV"],
	'jet1eta': [r"\eta_\mathrm{jet1}", ""],
	'zeta': [r"\eta_\mathrm{Z}", "GeV"],
	'zpt': [r"p_\mathrm{T}^\mathrm{Z}", "GeV"],
	'genzpt': [r"p_\mathrm{T}^\mathrm{gen Z}", "GeV"],
}

plots = []

#plotting function for variations


def twoD_all(quantity, datamc, opt):
	for variation_quantity in ['npv', 'jet1eta', 'zpt']:
		for change in utils.getvariationlist(variation_quantity, opt):
			twoD(quantity, datamc, opt, changes=change, folder=variation_quantity)


# Main 2d Plotting module
def twoD(quantity, files, opt, fig_axes=(), changes=None, settings=None):

	# if no settings are given, create:
	settings = utils.getsettings(opt, changes, settings, quantity)
	print "A %s plot is created with the following selection: %s" % (
		quantity, settings['selection'])

	#settings['legloc'] = "None"

	datamc, rootobjects = [], []
	settings['events'] = []
	for f in files:
		rootobjects += [getroot.histofromfile(quantity, f, settings, twoD=True)]
		#if (type(settings['rebin']) is list and len(settings['rebin']) == 2):
		#	rootobjects[-1].Rebin2D(settings['rebin'][0], settings['rebin'][1])
		#else:
		#	rootobjects[-1].Rebin2D(settings['rebin'], settings['rebin'])
		datamc += [mplconvert.root2histo(rootobjects[-1], f.GetName(), [1, 1])]
		settings['events'] += [datamc[-1].ysum()]

	if False:  # this is the code snippet to produce the diff plot for the rms methods
		method = 'ptbalance'
		quantity = "abs((recogen-%s)/recogen)*abs((recogen-%s)/recogen)_npv_zpt" % (method, method)
		rootobjects += [getroot.getobjectfromtree(quantity, f, settings, twoD=True)]
		#rootobjects[-1].Rebin2D(settings['rebin'], settings['rebin'])
		x = mplconvert.root2histo(rootobjects[-1], f.GetName(), [1, 1])
		datamc[-1].BinContents -= x.BinContents

	if settings['ratio'] and len(datamc) == 2:
		scaling_factor = datamc[0].binsum() / datamc[1].binsum()
		rootobjects[1].Scale(scaling_factor)
		rootobjects[0].Divide(rootobjects[1])
		rootobjects = [rootobjects[0]]
		datamc = [mplconvert.root2histo(rootobjects[0], f.GetName(), [1, 1])]

	if len(quantity.split("_")) == 2:
		# normalize to the same number of events
		if len(datamc) > 1 and settings['normalize']:
			for d in datamc[1:]:
				if d.binsum() > 0.0 and datamc[0].binsum() > 0:
					d.scale(datamc[0].binsum() / d.binsum())
		z_name = 'Events'
		if settings['z'] is None:
			settings['z'] = [0, np.max(datamc[0].BinContents)]
	else:
		if settings['z'] is None:
			settings['z'] = utils.getaxislabels_list(quantity.split("_")[0])[:2]
		if settings['xynames'] is not None and len(settings['xynames']) > 2:
			z_name = settings['xynames'][2]
		else:
			z_name = quantity.split("_")[0]

	# special dictionary for z-axis scaling (do we need this??)
	# 'quantity':[z_min(incut), z_max(incut), z_min(allevents), z_max(allevents)]
	z_dict = {
		'jet1pt': [0, 120, 0, 40],
		'jet2pt': [0, 40, 0, 40],
		'METpt': [15, 30, 15, 30],
		'ptbalance': [0.85, 1.1, 1, 4],
		'genzmass': [89, 93, 90.5, 92.5],
		'genzetarapidityratio': [1, 3, 0, 5]
	}

	#determine plot type: 2D Histogram or 2D Profile, and get the axis properties
	if settings['subplot'] == True:
		fig = fig_axes[0]
		grid = [fig_axes[1]]
	else:
		# create figure  + axes
		fig = plt.figure(figsize=(10. * len(datamc), 7.))
		grid = AxesGrid(fig, 111,
						nrows_ncols=(1, len(datamc)),
						axes_pad=0.4,
						share_all=True,
						aspect=False,
						label_mode="L",
						cbar_pad=0.2,
						cbar_location="right",
						cbar_mode='single',
						)

	for plot, label, ax in zip(datamc, settings['labels'], grid):
		ax.set_title(label)
		cmap1 = matplotlib.cm.get_cmap('jet')  # Blues
		ctext = 'white'
		if settings['fit'] and "profile" in settings['fit']:
			cmap1 = matplotlib.cm.get_cmap('Blues')  # Blues
			ctext = 'black'
		print settings['z']
		if "zlog" in settings and settings["zlog"]:  # TODO
			thenorm = matplotlib.colors.LogNorm()
			settings['z'][0] = min(settings['z'], 1)
		else:
			thenorm = None
		image = ax.imshow(plot.BinContents,
			interpolation='nearest',
			cmap=cmap1,
			origin='lower',
			aspect='auto',
			extent=[plot.xborderlow, plot.xborderhigh, plot.yborderlow, plot.yborderhigh],
			vmin=settings['z'][0],
			vmax=settings['z'][1],)  # norm=matplotlib.colors.LogNorm())

		# profiles in plot:
		if settings['fit'] and "profile" in settings['fit']:
			cprofile = ['green', 'red']
			xstr = xydict.get(settings['xynames'][0], [settings['xynames'][0], ""])[0]
			ystr = xydict.get(settings['xynames'][1], [settings['xynames'][1], ""])[0]
			profx = getroot.root2histo(rootobjects[datamc.index(plot)].ProfileX())
			profy = getroot.root2histo(rootobjects[datamc.index(plot)].ProfileY())
			ax.errorbar(profx.xc, profx.y, profx.yerr, fmt='v', label=r"Profile $%s$" % xstr, color=cprofile[0])
			ax.errorbar(profy.y, profy.xc, xerr=profy.yerr, fmt='<', label=r"Profile $%s$" % ystr, color=cprofile[1])

		# labels:
		labels.axislabels(ax, settings['xynames'][0], settings['xynames'][1],
															settings=settings)
		labels.labels(ax, opt, settings, settings['subplot'])
		utils.setaxislimits(ax, settings)
		if settings['fit'] and "correlation" in settings['fit']:
			ax.text(0.04, 0.92, r"correlation: ${0:.3f}$".format(
				rootobjects[datamc.index(plot)].GetCorrelationFactor()),
				color=ctext, transform=ax.transAxes)
		if settings['fit'] and "average" in settings['fit']:
			ax.text(0.04, 0.85, r"$\langle {0:s} \rangle = {2:.1f} \,\mathrm{{ {1:s} }}$".format(
				xydict.get(settings['xynames'][0], [settings['xynames'][0], ""])[0],
				xydict.get(settings['xynames'][0], [settings['xynames'][0], ""])[1],
				rootobjects[datamc.index(plot)].GetMean(1)),
				color=ctext, transform=ax.transAxes)
			ax.text(0.04, 0.80, r"$\langle {0:s} \rangle = {2:.1f} \,\mathrm{{ {1:s} }}$".format(
				xydict.get(settings['xynames'][1], [settings['xynames'][1], ""])[0],
				xydict.get(settings['xynames'][1], [settings['xynames'][1], ""])[1],
				rootobjects[datamc.index(plot)].GetMean(2)),
				color=ctext, transform=ax.transAxes)
	if settings['subplot']:
		return

	#add the colorbar
	cb = fig.colorbar(image, cax=grid.cbar_axes[0], ax=ax)
	cb.set_label(labels.unitformat(labels.getaxislabels_list(z_name)[2],
								labels.getaxislabels_list(z_name)[3], False))
	# create filename + folder
	settings['filename'] = utils.getdefaultfilename(quantity, opt, settings)

	utils.Save(fig, settings)


def ThreeD(files, opt, changes={}, rebin=[2, 2]):
	from mpl_toolkits.mplot3d import Axes3D

	change = plotbase.getchanges(opt, changes)
	change['incut'] = 'allevents'
	datamc = [getroot.getplotfromnick("2D_jet1eta_jet1phi", f, change, rebin) for f in files[1:]]

	# create supporting points
	x = np.linspace(-5, 5, 100 / rebin[0])
	y = np.linspace(-3.2, 3.2, 100 / rebin[1])
	X, Y = np.meshgrid(x, y)

	# create numpy array
	Z = np.array(datamc[0].BinContents)

	fig = plt.figure()
	ax = fig.add_subplot(111, projection='3d')

	#
	ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=matplotlib.cm.jet, linewidth=0)

	# set label + limits
	ax.set_zlim3d(0, datamc[0].maxBin())
	ax.set_xlabel(r'$\eta$')
	ax.set_ylabel(r'$\phi$')

	n = 360
	for i in range(n):

		# rotate viewing angle
		ax.view_init(20,-120 + (360 / n) * i)

		# create filename + save
		plotbase.Save(fig, str(i).zfill(3), opt)
