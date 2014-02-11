# -*- coding: utf-8 -*-
"""Plot quantities from data and MC.

	This module is dedicated to the simple comparison of plots from data and MC.
"""
import numpy
import math
import ROOT
import sys
import os
import copy

import Artus.Plotting.tools.getroot as getroot
import Artus.Plotting.tools.mplconvert as mplconvert
import Artus.Plotting.tools.utils as utils
import Artus.Plotting.tools.labels as labels
import Artus.Plotting.tools.fit as fit
import Artus.Plotting.tools.dictionaries as dictionaries


def datamcplot(quantity, files, opt, fig_axes=(), changes=None, settings=None):
	"""Template for all data/MC comparison plots for basic quantities."""

	# if no settings are given, create:
	settings = utils.getsettings(opt, changes, settings, quantity)
	print "A %s plot is created with the following selection: %s" % (quantity,
															settings['selection'])

	if 'flavour' in settings['xynames'][0]:
		settings['nbins'] = 25
	# create list with histograms from a ttree/tntuple
	datamc, rootobjects = [], []
	settings['events'] = []
	for f in files:
		rootobjects += [getroot.histofromfile(quantity, f, settings)]
		datamc += [mplconvert.root2histo(rootobjects[-1], f.GetName(), 1)]
		settings['events'] += [datamc[-1].ysum()]
		if 'flavour' in settings['xynames'][0]:
			datamc[-1].x = [5.5 if x == 20.5 else 6.5 if x == -0.5 else x for x in datamc[-1].x]
			datamc[-1].xc = [6 if x == 21 else 7 if x == 0 else x for x in datamc[-1].xc]
		#print datamc[-1]

	# if true, create a ratio plot:
	if settings['ratio'] and len(datamc) == 2:
		rootobject = getroot.rootdivision(rootobjects)
		datamc = [mplconvert.root2histo(rootobject, files[0].GetName(), 1)]
	else:
		rootobject = None

	# use the argument-given fig/axis or create new one:
	if settings['subplot'] == True:
		fig, ax = fig_axes
	else:
		fig, ax = utils.newplot(run=settings['run'])

	settings['filename'] = utils.getdefaultfilename(quantity, opt, settings)

	# create an additional ratio subplot at the bottom:
	if settings['ratiosubplot'] and not settings['subplot']:
		ratiosubplot(quantity, files, opt, settings)
		return

	# if runplot_diff, get the mean from mc:
	if settings['run'] == 'diff':
		datamc, ax, offset = runplot_diff(files, datamc, ax, settings, quantity)
	else:
		offset = 0

	# if true, save as root file:
	if settings['root'] is not False:
		getroot.saveasroot(rootobjects, opt, settings)
		return

	#loop over histograms: scale and plot
	for f, l, c, s, rootfile, rootobj in reversed(zip(datamc, settings['labels'],
					settings['colors'], settings['markers'], files, rootobjects)):
		scalefactor = 1
		if 'Profile' in f.classname:
			scalefactor = 1
			#s = s.replace('f','o')
		elif settings['normalize']:
			if 'scalefactor' in settings:
				f.scale(settings['scalefactor'])
			elif (f.ysum() != 0 and datamc[0].ysum() != 0):
				scalefactor = datamc[0].ysum() / f.ysum()
				f.scale(scalefactor)

		if s == 'f':
			wid = 0
			if len(f) > 1:
				wid = f.x[-1] - f.x[-2]
			f.append(f.x[-1] + wid, f.x[-1] + wid, 0.0)

			if settings['special_binning']:
				widths = [(a - b) for a, b in zip(f.x[1:], f.x[:-1])]
				widths += [0]
			else:
				widths = (f.x[2] - f.x[1])
			ax.bar(f.x, f.y, widths, bottom=numpy.ones(len(f.x))
				* 1e-6, yerr=f.yerr, ecolor=c, label=l, fill=True, facecolor=c, edgecolor=c)
		else:
			ax.errorbar(f.xc, f.y, f.yerr, drawstyle='steps-mid', color=c,
													fmt=s, capsize=0, label=l, zorder=10)

		if settings['fit'] is not None and ("MC" not in l or settings['run'] is not "diff" and not settings['ratio'] ):
			fit.fit(ax, quantity, rootobj, settings, c, l,
											datamc.index(f), scalefactor, offset)

	# set the axis labels and limits
	labels.labels(ax, opt, settings, settings['subplot'])
	labels.axislabels(ax, settings['xynames'][0], settings['xynames'][1],
															settings=settings)
	utils.setaxislimits(ax, settings)
	if settings['xynames'][1] == 'events' and 'y' not in opt.user_options:
		ax.set_ylim(top=max(d.ymax() for d in datamc) * 1.2)

	#plot a vertical line at 1.0 for certain y-axis quantities:
	if ((settings['xynames'][1] in ['response', 'balresp', 'mpfresp', 'recogen',
							'ptbalance', 'L1', 'L2', 'L3', 'mpf', 'mpfresp']) or
							'cut' in settings['xynames'][1] or settings['ratio']):
		ax.axhline(1.0, color='black', linestyle=':')

	if settings['grid']:
		ax.grid(True)

	if 'flavour' in settings['xynames'][0]:
		ax.set_xlim(0.5, 7.5)
		ax.set_xticks([1, 2, 3, 4, 5, 6, 7])
		ax.set_xticklabels(['d', 'u', 's', 'c', 'b', 'g', 'undef.'])
		ax.axvline(6.5, color='black', linestyle=':')

	if settings['log']:
		if 'y' not in opt.user_options:
			ax.set_ylim(bottom=1.0, top=max(d.ymax() for d in datamc) * 2)
		ax.set_yscale('log')

	# save it
	if settings['subplot']:
		del rootobjects
		return
	else:
		utils.Save(fig, settings)

try:
	datamcplot = profile(datamcplot)
except NameError:
	pass  # not running with profiler, that's ok.


def runplot_diff(files, datamc, ax, settings, quantity):

	settings2 = copy.deepcopy(settings)
	if 'components' in settings['xynames'][1]:
		settings2['x'] = [0, 1.5]
	else:
		settings2['x'] = plotbase.getaxislabels_list(settings['xynames'][1])[:2]

	mc = getroot.getobjectfromtree(quantity.split("_")[0], files[1], settings2)
	offset = mc.GetMean()

	new_y = []
	for x_elem, y_elem, yerr_elem in zip(datamc[0].xc, datamc[0].y,
																datamc[0].yerr):
		if y_elem == 0.0:
			datamc[0].xc.remove(x_elem)
			datamc[0].y.remove(y_elem)
			datamc[0].yerr.remove(yerr_elem)
		else:
			new_y.append(y_elem - offset)

	datamc[0].y = new_y
	#ax.axhspan(mc.GetMeanError(),-mc.GetMeanError(),
	#									color=settings2['colors'][0], alpha=0.4)
	return datamc, ax, offset


#Some additional submodules ...
def datamc_all(quantity, files, opt, fig_axes=(), subplot=False,
											changes=None, settings=None):
	"""Plot subplots of one quantity in bins of different variation.
		Loop over the different variations and the different alpha cut values.

		plotbase.getvariationlist gets a list of 'changes' dictionaries, one for each variation bin:
		ch_list = [{'var': 'var_JetEta_0to0_783'}, {'var': 'var_JetEta_0_783to1_305'}, ...]

		fig_axes = plotbase.newplot(subplots=n) creates a plot figure (fig_axes[0]) with a list fig_axes[1]
		of n 'axes' elements (=subplots), where fig_axes[1][n] is the n-th subplot
	"""
	if 'run' in quantity:
		run = True
		variations = ['eta', 'zpt']
		rebin = 500
		files = [d for d, name in zip(files, opt.files) if 'data' in name]
	else:
		variations = ['npv', 'jet1eta', 'zpt', 'alpha']

	subtexts = plotbase.getdefaultsubtexts()

	# change this
	run = False

	if quantity in variations:
		variations.remove(quantity)
	for variation in variations:
		for cut, cut_string in zip(opt.cut, getroot.cutstrings(opt.cut)):
			ch_list = plotbase.getvariationlist(variation, opt)
			fig_axes = plotbase.newplot(subplots=len(ch_list), run=run)
			for ch, ax, subtext in zip(ch_list, fig_axes[1], subtexts):
				if variation == 'zpt':
					ch['var'] = cut_string
				elif variation is not 'alpha':
					ch['var'] = cut_string + "_" + ch['var']
				if run:
					runplot(quantity, files, opt, changes=ch, fig_axes=(fig_axes[0], ax), subplot=True, log=log, subtext=subtext, rebin=rebin, legloc=legloc, fit=fit)
				else:
					datamcplot(quantity, files, opt, changes=ch, subplot=True,
									fig_axes=(fig_axes[0], ax), settings=settings)

			if variation == 'alpha':
				text = " for different " + plotbase.nicetext(variation) + " values "
			else:
				text = " in " + plotbase.nicetext(variation) + " bins for " + r"$\alpha$ " + str(cut) + "  "
			title = plotbase.nicetext(quantity) + text + opt.algorithm + " " + opt.correction
			fig_axes[0].suptitle(title, size='x-large')

			if variation == 'alpha':
				text = "_bins__"
			else:
				text = "_bins__alpha_" + str(cut).replace('.', '_') + "__"
			filename = quantity + "/" + quantity + "_in_" + variation + text + opt.algorithm + opt.correction
			plotbase.EnsurePathExists(opt.out + "/" + quantity)
			plotbase.Save(fig_axes[0], filename, opt)
			if variation == 'alpha':
				break


def ratiosubplot(quantity, files, opt, settings):

	fig = plotbase.plt.figure(figsize=[7, 10])
	ax1 = plotbase.plt.subplot2grid((3, 1), (0, 0), rowspan=2)
	ax1.number = 1
	ax2 = plotbase.plt.subplot2grid((3, 1), (2, 0))
	ax2.number = 2
	fig.add_axes(ax1)
	fig.add_axes(ax2)

	changes = {'subplot': True}

	datamcplot(quantity, files, opt, fig_axes=(fig, ax1), changes=changes, settings=settings)

	changes['energy'] = None
	changes['lumi'] = None
	changes['ratio'] = True
	changes['legloc'] = False
	changes['y'] = [0.5, 1.5]
	changes['labels'] = ['Ratio']
	changes['xynames'] = [settings['xynames'][0], 'datamcratio']

	datamcplot(quantity, files, opt, fig_axes=(fig, ax2), changes=changes, settings=settings)

	fig.subplots_adjust(hspace=0.05)
	ax1.set_xticks([])
	ax1.set_xlabel("")

	plotbase.Save(fig, settings)


def plotfromdict(datamc, opt, name, blacklist=[]):
	if len(dictionaries.d_plots[name]) == 0:  # emptylist
		datamcplot(name, datamc, opt)

	elif len(dictionaries.d_plots[name]) == 1:  # list contains only arguments
		eval("datamcplot('" + name + "', datamc, opt, " + dictionaries.d_plots[name][0] + ")")

	elif len(dictionaries.d_plots[name]) == 2:  # list contains arguments+function
		eval(dictionaries.d_plots[name][1] + "('" + name + "', datamc, opt, " + dictionaries.d_plots[name][0] + ")")

	elif len(dictionaries.d_plots[name]) == 3:  # list contains arguments+function+name
		eval(dictionaries.d_plots[name][1] + "('" + dictionaries.d_plots[name][2] + "', datamc, opt, " + dictionaries.d_plots[name][0] + ")")


plots = []

if __name__ == "__main__":
	"""Unit test: doing the plots standalone (not as a module)."""
	import sys
	if len(sys.argv) < 2:
		print "Usage: python plotting/plotdatamc.py data_file.root mc_file.root"
		exit(0)
	datamc[0] = getroot.openfile(sys.argv[1])
	fmc = getroot.openfile(sys.argv[2])
	bins = getroot.getbins(datamc[0], [])
	zpt(datamc, opt=plotbase.options(bins=bins))
	jeteta(datamc, opt=plotbase.options(bins=bins))
	cut_all_npv(datamc, opt=plotbase.options(bins=bins))
