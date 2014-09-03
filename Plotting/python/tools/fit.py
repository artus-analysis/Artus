# -*- coding: utf8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import HarryPlotter.Plotting.tools.getroot as getroot
import HarryPlotter.Plotting.tools.mplconvert as mplconvert

import numpy as np
import array
import ROOT
import numpy
import math
import matplotlib.mlab as mlab


def fit(rootobject, plotdict):
	"""One of several fits is added to an axis element, fit parameters are added as text element"""
	if not plotdict['fit']:
		return

	index = plotdict['roothistos'].index(rootobject)
	color = plotdict['colors'][index]

	if plotdict['fit'] == 'vertical':
		plotdict['axes'].axvline(rootobject.GetMean(), color=color, linestyle='-')
		plotdict['axes'].axvspan(rootobject.GetMean() - rootobject.GetMeanError(), 
				rootobject.GetMean() + rootobject.GetMeanError(),
				color=color, alpha=0.1)
		plotdict['axes'].text(0.5, 0.97 - (index / 20.),
			r"$\langle \mathrm{%s} \rangle = %1.4f\pm%1.4f$" % (plotdict['labels'][index],
				rootobject.GetMean(), rootobject.GetMeanError()),
			 va='top', ha='right', transform=plotdict['axes'].transAxes,
			 color=color)

		plotdict['axes'].text(0.97, 0.97 - (index / 20.), 
			r"$\mathrm{RMS (%s)} = %1.3f$" % (plotdict['labels'][index],
			rootobject.GetRMS()), va='top', ha='right', 
			transform=plotdict['axes'].transAxes,
			color=color)

	elif plotdict['fit'] == 'chi2':
		# fit a horizontal line
		intercept, ierr, chi2, ndf = fitline(rootobject, plotdict['xlims'])
		plotdict['axes'].axhline(intercept, color=color, linestyle='--')
		plotdict['axes'].axhspan(intercept + ierr, intercept - ierr, color=color, alpha=0.2)

		# and display chi^2
		plotdict['axes'].text(0.97, 0.15 - (index / 10.), r"y average: %1.3f$\pm$%1.3f" % (intercept, ierr),
		va='top', ha='right', transform=plotdict['axes'].transAxes, color=color)
		plotdict['axes'].text(0.97, 0.10 - (index / 10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
		va='top', ha='right', transform=plotdict['axes'].transAxes, color=color)

	elif plotdict['fit'] == 'gauss':
		p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = fitline2(rootobject, gauss=True, limits=plotdict['xlims'])

		x = numpy.linspace(plotdict['xlims'][0], plotdict['xlims'][1], 500)
		ymax = max(mlab.normpdf(x, p1, p2))

		plotdict['axes'].plot(x, p0 / ymax * mlab.normpdf(x, p1, p2), color=color)

		plotdict['axes'].text(0.03, 0.97 - (index / 20.), r"$\mathrm{%s:}$" % plotdict['labels'][index],
			   va='top', ha='left', transform=plotdict['axes'].transAxes, color=color)
		plotdict['axes'].text(0.25, 0.97 - (index / 20.), r"$\mu = %1.2f\pm%1.2f$" % (p1, p1err),
			   va='top', ha='left', transform=plotdict['axes'].transAxes, color=color)
		plotdict['axes'].text(0.60, 0.97 - (index / 20.), r"$\sigma = %1.2f\pm%1.2f$" % (p2, p2err),
			   va='top', ha='left', transform=plotdict['axes'].transAxes, color=color)
	elif type(plotdict['fit']) == str and 'quadratic' in plotdict['fit']:
		pa, paerr, pb, pberr, pc, pcerr, chi2, ndf, conf_intervals = fitline2(rootobject, quadratic=True)

		def func(x):
			return pc * x * x + pb * x + pa
		mean = rootobject.GetMean()

		conf_intervals = [conf_intervals[0]] + conf_intervals + [conf_intervals[-1]]

		# insert a (0, 0) bin because the conf_intervals list also contains an additional (0., conf)-point
		plot = mplconvert.root2histo(rootobject)
		plot.xc.insert(0, 0.)

		# fit line:
		xvals = [plotdict['xlims'][0]] + plot.xc + [plotdict['xlims'][1]]

		line_fit = plotdict['axes'].plot(xvals, [func(x) for x in xvals], color=color, linestyle='-')
		# display confidence intervals
		plotdict['axes'].fill_between(xvals,
			  [(func(x) - c) for x, c in zip(xvals, conf_intervals)],
			  [(func(x) + c) for x, c in zip(xvals, conf_intervals)],
			  facecolor=color, edgecolor=color, interpolate=True, alpha=0.2)
		if "labels" in plotdict['fit']:
			qstr = xdict.get(quantity.split('_')[-1], "x")
			plotdict['axes'].text(0.05, 0.950 - (index / 15.), r"$f({3:s}) = {0:.3f} {1:+.3f}\,{3:s} {2:+.3f}\,{3:s}^2$".format(pa, pb, pc, qstr),
			  va='top', ha='left', transform=plotdict['axes'].transAxes, color=color)
			log.debug(pa + " " + pb + " " + pc)
			log.debug(paerr + " " + pberr + " " + pcerr)
		if "chi2" in plotdict['fit']:
			plotdict['axes'].text(0.05, 0.20 - (index / 10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
			  va='top', ha='left', transform=plotdict['axes'].transAxes, color=color)

	else:
		intercept, ierr, slope, serr,  chi2, ndf, conf_intervals = fitline2(rootobject, quadratic=(plotdict['fit']=='quadratic'))
		mean = rootobject.GetMean()

		# fit line:
		line_fit = plotdict['axes'].plot(plotdict['xlims'][:2],[(intercept + plotdict['xlims'][0] * slope), 
			(intercept+plotdict['xlims'][1]*slope)], color=color, linestyle='--')

		# insert a (0, 0) bin because the conf_intervals list also contains an additional (0., conf)-point
		plot = mplconvert.root2histo(rootobject)
		plot.xc.insert(0, 0.)

		# display confidence intervals
		plotdict['axes'].fill_between(plot.xc,
			  [(intercept + x * slope + c) for x, c in zip(plot.xc, conf_intervals)],
			  [(intercept+x*slope - c) for x, c in zip(plot.xc, conf_intervals)],
			  facecolor=color, edgecolor=color, interpolate=True, alpha=0.2)

		#display slope:
		if plotdict['fit'] == 'slope':
			if 'fit_slope_exponent' in plotdict:
				fit_slope_exponent = plotdict['fit_slope_exponent']
			else:
				try:
					fit_slope_exponent = int(math.log10(abs(slope))) - 1
					plotdict['fit_slope_exponent'] = fit_slope_exponent
				except:
					return
			plotdict['axes'].text(0.97, 0.95 - (index / 10.), r"$\mathrm{Fit\/slope} = (%1.2f\pm%1.2f) \times 10^{%g}$" % (slope/(10**fit_slope_exponent), serr/(10**fit_slope_exponent), fit_slope_exponent),
			   va='top', ha='right', transform=plotdict['axes'].transAxes, color=color,
			   size='x-large')
		elif plotdict['fit'] == 'intercept':
			#display intercept ...
			plotdict['axes'].text(0.97, 0.35 - (index / 10.), r"$\mathrm{y(0)} = %1.4f\pm%1.4f$" % (intercept, conf_intervals[0]),
			   va='top', ha='right', transform=plotdict['axes'].transAxes, color=color, size='x-large')

			# ... and chi2 (smaller)
			plotdict['axes'].text(0.97, 0.30 - (index / 10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
				va='top', ha='right', transform=plotdict['axes'].transAxes, color=color, size='small')


def fitline(rootgraph, limits=[0, 1]):
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate()  # call this function so zero-error (one entry) bins don't distort the fit
	fitf = ROOT.TF1("fit1", "1*[0]", *limits)
	fitres = rootgraph.Fit(fitf, "SQN")
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate(0)  # change back after fit
	return (fitf.GetParameter(0), fitf.GetParError(0), fitres.Chi2(), fitres.Ndf())


def fitline2(rootgraph, quadratic=False, gauss=False, limits=[0, 1000]):
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate()  # call this function so zero-error (one entry) bins don't distort the fit
	if quadratic:
		f = "1*[0]+x*[1]+x*x*[2]"
	elif gauss:
		f = "gaus"
	else:
		f = "1*[0]+x*[1]"
	fitf = ROOT.TF1("fit1", f, *limits)
	fitres = rootgraph.Fit(fitf, "SQN")
	#Get conf intervals as an array and then convert to list
	points = []
	for n in range(1, rootgraph.GetSize() - 1):
		points.append(rootgraph.GetBinCenter(n))
	#add additional points at the beginning and end to extend the fit error bands
	points.insert(0, 0.)
	points.append(2 * points[-1] - points[-2])
	x = array.array('d', points)
	y = array.array('d', [0.] * len(points))
	fitres.GetConfidenceIntervals(len(points), 1, 1, x, y, 0.683)
	conf_intervals = [i for i in y]

	if quadratic or gauss:
		return (fitf.GetParameter(0), fitf.GetParError(0),
			fitf.GetParameter(1), fitf.GetParError(1),
			fitf.GetParameter(2), fitf.GetParError(2),
			fitres.Chi2(), fitres.Ndf(), conf_intervals)
	else:
		return (fitf.GetParameter(0), fitf.GetParError(0),
			fitf.GetParameter(1), fitf.GetParError(1),
			fitres.Chi2(), fitres.Ndf(), conf_intervals)
