import Artus.Plotting.tools.getroot as getroot

import numpy as np
import array
import ROOT
import numpy
import math
import matplotlib.mlab as mlab

xdict = {  # x labels in latex for fit functions
	'npv' : r"n_\mathrm{PV}",
}

def fit(ax, quantity, rootobject, settings, color='black', label="", index=0, 
		scalefactor=1, offset=None):
	"""One of several fits is added to an axis element, fit parameters are added as text element"""
	if not settings['fit']:
		return

	if color in ['#CBDBF9', 'lightskyblue']: color = 'blue'

	limits = settings['x']


	if settings['fit']=='vertical' and label != "Ratio":
		if quantity == 'METpt': unit = r' \/ / \/ GeV'
		else: unit = ""

		if 'fitlabel_offset' in settings:
			offset += settings['fitlabel_offset']

		ax.axvline(rootobject.GetMean(), color=color, linestyle='-')
		ax.axvspan(rootobject.GetMean()-rootobject.GetMeanError(), rootobject.GetMean()+rootobject.GetMeanError(), color=color, alpha=0.1)
		ax.text(0.5, 0.97-(index/20.)+offset, r"$\langle \mathrm{%s} \rangle = %1.4f\pm%1.4f" % (label, rootobject.GetMean(), rootobject.GetMeanError())+"\mathrm{%s}$" % unit,
			   va='top', ha='right', transform=ax.transAxes, color=color)

		ax.text(0.97, 0.97-(index/20.)+offset, r"$\mathrm{RMS (%s)} = %1.3f$" % (label, rootobject.GetRMS()), va='top', ha='right', transform=ax.transAxes, color=color)

	elif settings['fit'] == 'chi2':
		# fit a horizontal line
		intercept, ierr, chi2, ndf = fitline(rootobject, limits)
		#account for scaling and rebinning:
		intercept = scalefactor * intercept
		ax.axhline(intercept, color=color, linestyle='--')
		ax.axhspan(intercept+ierr, intercept-ierr, color=color, alpha=0.2)

		# and display chi^2
		ax.text(0.97, 0.15-(index/10.), r"y average: %1.3f$\pm$%1.3f" % (intercept, ierr),
		va='top', ha='right', transform=ax.transAxes, color=color)
		ax.text(0.97, 0.10-(index/10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
		va='top', ha='right', transform=ax.transAxes, color=color)
		 

	elif settings['fit'] == 'gauss':
		p0, p0err, p1, p1err, p2, p2err, chi2, ndf, conf_intervals = fitline2(rootobject, gauss=True, limits=limits)

		x=numpy.linspace(limits[0], limits[1], 500)			
		ymax = max(mlab.normpdf(x, p1, p2))

		ax.plot(x,scalefactor * p0 / ymax * mlab.normpdf(x, p1, p2), color = color)

		ax.text(0.03, 0.97-(index/20.)+offset, r"$\mathrm{%s:}$" % label,
			   va='top', ha='left', transform=ax.transAxes, color=color)
		ax.text(0.20, 0.97-(index/20.)+offset, r"$\mu = %1.4f\pm%1.4f$" % (p1, p1err),
			   va='top', ha='left', transform=ax.transAxes, color=color)
		ax.text(0.60, 0.97-(index/20.)+offset, r"$\sigma = %1.4f\pm%1.4f$" % (p2, p2err),
			   va='top', ha='left', transform=ax.transAxes, color=color)
	elif type(settings['fit']) == str and 'quadratic' in settings['fit']:
		pa, paerr, pb, pberr, pc, pcerr, chi2, ndf, conf_intervals = fitline2(rootobject, quadratic=True)
		def func(x):
			return pc*x*x + pb*x + pa
		mean = rootobject.GetMean()
		#intercept -= offset
		conf_intervals = [conf_intervals[0]] + conf_intervals + [conf_intervals[-1]]
		#print conf_intervals
		#exit()

		# insert a (0, 0) bin because the conf_intervals list also contains an additional (0., conf)-point
		plot = getroot.root2histo(rootobject)
		plot.xc.insert(0, 0.)
 
		# fit line:
		xvals = [limits[0]] + plot.xc + [limits[1]]

		line_fit = ax.plot(xvals ,[func(x)*scalefactor for x in xvals], color = color, linestyle='-')
		# display confidence intervals
		ax.fill_between(xvals, 
			  [(func(x)-c)*scalefactor for x, c in zip(xvals, conf_intervals)],
			  [(func(x)+c)*scalefactor for x, c in zip(xvals, conf_intervals)], 
			  facecolor=color, edgecolor=color, interpolate=True, alpha=0.2)
		if "labels" in settings['fit']:
			qstr = xdict.get(quantity.split('_')[-1], "x")
			ax.text(0.05, 0.950-(index/15.), r"$f({3:s}) = {0:.3f} {1:+.3f}\,{3:s} {2:+.3f}\,{3:s}^2$".format(pa, pb, pc, qstr),
			  va='top', ha='left', transform=ax.transAxes, color=color)
			print pa, pb, pc
			print paerr, pberr, pcerr
		if "chi2" in settings['fit']:
			ax.text(0.05, 0.20-(index/10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
			  va='top', ha='left', transform=ax.transAxes, color=color)
			#ax.text(0.05, 0.65-(index/10.), r"y average: %1.3f$\pm$%1.3f" % (pc, pb),
			#  va='top', ha='left', transform=ax.transAxes, color=color)
			#ax.text(0.05, 0.60-(index/10.), r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
			#  va='top', ha='left', transform=ax.transAxes, color=color)

	elif label != "Ratio":
		intercept, ierr, slope, serr,  chi2, ndf, conf_intervals = fitline2(rootobject, quadratic=(settings['fit']=='quadratic'))
		mean = rootobject.GetMean()
		intercept -= offset

		# fit line:
		line_fit = ax.plot(limits[:2],[(intercept+limits[0]*slope)*scalefactor, 
			(intercept+limits[1]*slope)*scalefactor], color = color, linestyle='--')

		# insert a (0, 0) bin because the conf_intervals list also contains an additional (0., conf)-point
		plot = getroot.root2histo(rootobject)
		plot.xc.insert(0, 0.)

		# display confidence intervals
		ax.fill_between(plot.xc, 
			  [(intercept+x*slope + c)*scalefactor for x, c in zip(plot.xc, conf_intervals)],
			  [(intercept+x*slope - c)*scalefactor for x, c in zip(plot.xc, conf_intervals)], 
			  facecolor=color, edgecolor=color, interpolate=True, alpha=0.2)

		if 'fitlabel_offset' in settings:
			offset += settings['fitlabel_offset']

		#display slope:
		if settings['fit'] == 'slope':
			if 'fit_slope_exponent' in settings:
				fit_slope_exponent = settings['fit_slope_exponent']
			else:
				try:
					fit_slope_exponent = int(math.log10(abs(slope)))-1
					settings['fit_slope_exponent'] = fit_slope_exponent				
				except:
					return
			ax.text(0.97, 0.95-(index/10.)+offset, r"$\mathrm{Fit\/slope} = (%1.2f\pm%1.2f) \times 10^{%g}$" % (slope/(10**fit_slope_exponent), serr/(10**fit_slope_exponent), fit_slope_exponent),
			   va='top', ha='right', transform=ax.transAxes, color=color,
			   size='x-large')
		elif settings['fit'] == 'intercept':
			#display intercept ...
			ax.text(0.97, 0.35-(index/10.)+offset, r"$\mathrm{y(0)} = %1.4f\pm%1.4f$" % (intercept, conf_intervals[0]),
			   va='top', ha='right', transform=ax.transAxes, color=color, size='x-large')

			# ... and chi2 (smaller)
			ax.text(0.97, 0.30-(index/10.)+offset, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
				va='top', ha='right', transform=ax.transAxes, color=color, size='small')
		elif settings['fit'] == 'chi2_linear':
			pass
			# display chi2
			#ax.text(0.97, 0.20-(index/10.)+offset, r"$\chi^2$ / n.d.f. = {0:.2f} / {1:.0f} ".format(chi2, ndf),
			#	va='top', ha='right', transform=ax.transAxes, color=color)#, size='x-large')


def fitline(rootgraph, limits=[0, 1]):
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate()  # call this function so zero-error (one entry) bins don't distort the fit
	fitf = ROOT.TF1("fit1", "1*[0]", *limits)
	fitres = rootgraph.Fit(fitf,"SQN")
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate(0)  # change back after fit
	return (fitf.GetParameter(0), fitf.GetParError(0), fitres.Chi2(), fitres.Ndf())


def fitline2(rootgraph, quadratic=False, gauss=False, limits = [0, 1000]):
	if 'Profile' in rootgraph.ClassName():
		rootgraph.Approximate() # call this function so zero-error (one entry) bins don't distort the fit
	if quadratic:
		f = "1*[0]+x*[1]+x*x*[2]"
	elif gauss:
		f = "gaus"
	else:
		f = "1*[0]+x*[1]"
	fitf = ROOT.TF1("fit1", f, *limits)
	fitres = rootgraph.Fit(fitf,"SQN")
	#Get conf intervals as an array and then convert to list
	points = []
	for n in range(1, rootgraph.GetSize() - 1):
		points.append(rootgraph.GetBinCenter(n))
	#add additional points at the beginning and end to extend the fit error bands
	points.insert(0, 0.)
	points.append(2*points[-1]-points[-2])
	x = array.array('d', points)
	y = array.array('d', [0.]*len(points))
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


