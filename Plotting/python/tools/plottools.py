#!/usr/bin/env python
# This file is not used in merlin, but it is very nice and can serve as source of inspiration
# It is written by Fred Stober

import numpy, md5, os, sys, math
sys.path.insert(1, os.path.expanduser('~/matplotlib'))

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.font_manager import FontProperties

nick = 'key_nick'
data = 'key_data'
color = 'key_color'
label = 'key_label'
style = 'key_style'
vis = 'key_vis'
alpha = 'key_alpha'
notes = 'key_notes'
preset = 'key_preset'

plot_presets = {
	'mc-a1': {color: 'r', 'fmt': 'o'},
	'mc-a2': {color: 'b', 'fmt': '#o'},
	'mc-b1': {color: 'r', 'fmt': 's'},
	'mc-b2': {color: 'b', 'fmt': '#s'},
	'mc-c1': {color: 'g', 'fmt': 'D'},
	'mc-c2': {color: 'y', 'fmt': '#D'},
}

getRX = lambda data, pad = 0, rpad = 0: ((1-rpad)*(min(data['x'])-pad), (1+rpad)*(max(data['x'])+pad))
getRY = lambda data, pad = 0, rpad = 0: ((1-rpad)*(min(data['y'])-pad), (1+rpad)*(max(data['y'])+pad))

from iotools import *
from histocalc import scaleData, getRelativeError

def hsv2rgb(h, s, v):
	return tuple(matplotlib.colors.hsv_to_rgb(numpy.array([[(h, s, v)]]))[0][0])

def getCMSFigure(opts):
	if opts.get('square', False):
		fig = plt.figure(figsize=(6*0.7,6*0.7))
	else:
		fig = plt.figure(figsize=(8.4854*0.7,6*0.7))
	fig.subplots_adjust(
		left=opts.get('left', 0.15),
		right=1-opts.get('right', 0.05),
		top=1-opts.get('top', 0.08),
		bottom=opts.get('bottom', 0.15))
	return fig


def getFigure(opts):
#	fig = plt.figure(figsize=(8.4854,6))
	if opts.get('square', False):
		fig = plt.figure(figsize=(6*0.7,6*0.7))
	else:
		fig = plt.figure(figsize=(8.4854*0.7,6*0.7))
#	fig.subplots_adjust(left=0.1, right=1-0.025, top=1-0.025, bottom=0.1)
	fig.subplots_adjust(left=opts.get('padleft', 0.15), right=1-0.05, top=1-0.05, bottom=0.15)
#	print plt.figaspect(2.)
#	if hasattr(opts, 'slides') and opts.slides:
#		matplotlib.rc('font', size=16)
#		fig = plt.figure(figsize=(6,4))
#		fig.subplots_adjust(hspace=0.2, wspace=0.2, left=0.2, right=1-0.025, top=1-0.025, bottom=0.2)
#	else:
#		fig = plt.figure(figsize=(8.4854,6))
#		fig.subplots_adjust(hspace=0.2, wspace=0.2, right=1-0.025, top=1-0.025)
#	if hasattr(opts, 'fsize') and opts.fsize:
#		matplotlib.rc('font', size=opts.fsize)
	return fig


def setupAxis(ax, opts, xprefix = 'x', yprefix = 'y'):
	def setupAxis_single(ax_int, prefix):
		if opts.get(prefix + 'labelsize', None):
			ax_int.label.set_size(opts.get(prefix + 'labelsize'))
		ax_int.label.set_color(opts.get(prefix + 'color', 'black'))

		scale = opts.get(prefix + 'scale', 'linear')
		axlim = opts.get(prefix + 'range')
		if prefix.startswith('x'):
			if axlim:
				ax.set_xlim(*axlim)
			ax.set_xscale(scale)
		elif prefix.startswith('y'):
			if axlim:
				ax.set_ylim(*axlim)
			ax.set_yscale(scale)

		tmp = opts.get(prefix + 'ticks_manual', None)
		if tmp:
			ax_int.set_ticks(tmp['ticks'])
			ax_int.set_ticklabels(tmp['labels'],
				rotation = tmp.get('rotation', 0),
				fontsize = tmp.get('size', None),
				colors = opts.get(prefix + 'color', 'black'))
		elif scale == 'log':
			setupLogTicks(ax_int, opts.get(prefix + 'ticks', [1,2,5]))
		elif opts.get(prefix + 'scaletex', True):
			prec = str(opts.get(prefix + 'prec', 1))
			ax_int.set_major_formatter(matplotlib.ticker.FormatStrFormatter(r'$%.' + prec + 'f$'))
			for x in ax_int.get_ticklabels():
				x.set_color(opts.get(prefix + 'color', 'black'))
#			ax.xaxis.set_minor_locator(matplotlib.ticker.LogLocator(base=10.0,subs=[0,1,2,3,4,5,6,7,8,9]))
#			ax.xaxis.set_major_locator(matplotlib.ticker.LogLocator(base=10.0,subs=[0,1,2,3,4,5,6,7,8,9]))

	ax.set_xlabel(opts.get(xprefix + 'label', 'x'), ha = "right", x = 1)
	setupAxis_single(ax.xaxis, xprefix)

	ax.set_ylabel(opts.get(yprefix + 'label', 'y'), va = "top", y = 1)
	setupAxis_single(ax.yaxis, yprefix)

	ax.grid(opts.get('grid', False))
	return ax


def getAxis(opts, fig, pos = 111, xshare = None, yshare = None):
	args = {}
	if 'xrange' in opts:
		args['xlim'] = opts.get('xrange')
	if 'yrange' in opts:
		args['ylim'] = opts.get('yrange')
	if len(args) > 0:
		args['autoscale_on'] = False
	if xshare:
		args['sharex'] = xshare
	if yshare:
		args['sharey'] = yshare
	if 'title' in opts:
		args['title'] = opts.get('title')

	if isinstance(pos, tuple):
		ax = fig.add_subplot(*pos, **args)
	else:
		ax = fig.add_subplot(pos, **args)
	return setupAxis(ax, opts)


def getLabelProxy(p, pinfo):
	if type(p) is matplotlib.collections.PolyCollection:
		try:
			return matplotlib.patches.Rectangle((0, 0), 1, 1,
				lw = p.get_linewidth()[0], fc = p.get_facecolor()[0],
				ec = p.get_edgecolor()[0], alpha = p.get_alpha())
		except:
			return None
	else:
		try:
			marker = p[0].get_marker()
			linestyle = p[0].get_linestyle()
			if str(linestyle) == 'None':
				linestyle = '-'
			return matplotlib.lines.Line2D([0,1], [1,1], color=p[0].get_color(),
				linewidth=p[0].get_linewidth(), linestyle=linestyle,
				marker=marker, markersize=p[0].get_markersize() + 3,
				markeredgewidth=0.5, markerfacecolor=p[0].get_markerfacecolor(),
				markeredgecolor='k')
		except:
			return matplotlib.patches.Rectangle((0, 0), 1, 1,
				fc = p[0].get_facecolor(), ec = p[0].get_facecolor())


def drawLegend(ax, plots, opts):
	legends = set()
	for plot in plots:
		legends.add(plot.get('legend', None))
	leg_vis = None
	for leg in legends:
		if leg_vis != None:
			ax.add_artist(leg_vis)
		name = 'legend'
		if leg != None:
			name = 'legend_%s' % leg
		leg_plots = filter(lambda p: p.get('legend', None) == leg, plots)
		leg_vis = drawLegend_int(ax, leg_plots, **opts.get(name, {}))
		if opts.get(name, {}).get('width', 0):
			box = ax.get_position()
			ax.set_position([box.xmin, box.ymin, box.width*(1-opts.get(name, {}).get('width', 0)), box.height])


def getPlotLabel(plot):
	if (label in plot) and ('meta' in plot[data]):
		return plot[label] % plot[data]['meta']
	return plot.get(label)


def drawLegend_int(ax, plots, size = 12, loc = 0, cols = 1, bbox_to_anchor = None, **other):
	plots = filter(lambda p: p.get(label), plots)
#	proxy = map(lambda p: p[vis], plots)
	proxy = map(lambda p: getLabelProxy(p[vis], p), plots)
	labels = map(getPlotLabel, plots)
	locMap = {1: 3, 2: 8, 3: 4, 4: 6, 5: 10, 6: 7, 7: 2, 8: 9, 9: 1, 0: 0}
	loc = int(loc)
	loc = locMap[loc]
	if len(filter(lambda l: l != None, labels)) > 0:
		if bbox_to_anchor:
			return ax.legend(proxy, labels, prop=FontProperties(size=size),
				handlelength=2.5, loc=loc, bbox_to_anchor=bbox_to_anchor, ncol=cols, numpoints=1)
		else:
			return ax.legend(proxy, labels, prop=FontProperties(size=size),
				handlelength=2.5, loc=loc, ncol=cols, numpoints=1)


def mergeOpts(opts, kwargs):
	opts = dict(opts)
	opts.update(kwargs)
	return opts


def setupSimplePlot(opts, **kwargs):
	opts = mergeOpts(opts, kwargs)
	fig = getFigure(opts)
	ax = getAxis(opts, fig, 111)
	return (fig, ax)


def setupCMSPlot(opts, **kwargs):
	opts = mergeOpts(opts, kwargs)
	fig = getCMSFigure(opts)
	ax = getAxis(opts, fig, 111)
	return (fig, ax)


def setupDualPlot(opts, xrange = None, yrange1 = None, yrange2 = None, **kwargs):
	opts = mergeOpts(opts, kwargs)

	fig = getFigure(opts)
	fig.subplots_adjust(hspace=0.1, left=opts.get('padleft', 0.12), right=1-0.025, top=1-0.025, bottom=opts.get('padbottom', 0.1))
	if xrange:
		opts['xrange'] = xrange

	if yrange1:
		opts['yrange'] = yrange1
	elif opts.get('yrange1'):
		opts['yrange'] = opts.get('yrange1')
	opts['yscale'] = opts.get('yscale1', 'linear')
	opts['ylabel'] = opts.get('ylabel1', 'y1')
	opts['yprec'] = opts.get('yprec1', 1)
	ax1 = getAxis(opts, fig, 211)

	if yrange2:
		opts['yrange'] = yrange2
	elif opts.get('yrange2'):
		opts['yrange'] = opts.get('yrange2')
	opts['yscale'] = opts.get('yscale2', 'linear')
	opts['ylabel'] = opts.get('ylabel2', 'y2')
	opts['yprec'] = opts.get('yprec2', 1)
	if opts.get('shareaxis', True):
		ax2 = getAxis(opts, fig, 212, xshare = ax1)
	else:
		ax2 = getAxis(opts, fig, 212)

	return (fig, ax1, ax2)


def setupRatioPlot(opts, xrange, yrange, rrange, **kwargs):
	if 'ylabel' in opts:
		opts['ylabel1'] = opts['ylabel']
	if 'yscale' in opts:
		opts['yscale1'] = opts['yscale']
	opts['yscale2'] = opts.get('rscale', 'linear')
	opts['ylabel2'] = opts.get('rlabel', 'Ratio')
	opts['yprec1'] = opts.get('yprec', 1)
	opts['yprec2'] = opts.get('rprec', 1)
	return setupDualPlot(opts, xrange, yrange, rrange, **kwargs)


def drawPlots(ax, plots, opts = {}, xy_switch = False):
	for plot_raw in plots:
		plot = dict(opts)
		plot.update(plot_raw)
		if preset in plot:
			plot.update(plot_presets[plot[preset]])
		plot[label] = getPlotLabel(plot)
		if 'fmt' in plot:
			if plot['fmt'].startswith('#'):
				plot['markerfacecolor'] = 'w'
				plot['fmt'] = plot['fmt'][1:]
		plot_data = {'x': plot[data]['x'], 'xe': plot[data]['xe']}
		plot_data['y'] = plot[data][plot.get('ysrc', 'y')]
		plot_data['ye'] = plot[data][plot.get('esrc', 'ye')]
		if xy_switch:
			plot_data = {'x': plot_data['y'], 'xe': plot_data['ye'], 'y': plot_data['x'], 'ye': plot_data['xe']}

		# Expand asymmetric error bars
		if plot_data['ye'].ndim == 2:
			(ye_low, ye_high) = (plot_data['ye'][0], plot_data['ye'][1])
		else:
			(ye_low, ye_high) = (plot_data['ye'], plot_data['ye'])
		if ax.get_yscale() == 'log': # handle zero/subzero lower limits
			ye_low = numpy.where(ye_low > plot_data['y'], plot_data['y'] - ax.get_ylim()[0], ye_low)
		plot_data['ye'] = numpy.array([ye_low, ye_high])

		plotstyle = plot.get(style, 'errorbar')
		if plotstyle == 'steps':
			plot.setdefault('fmt', '')
			plot.setdefault('drawstyle', 'steps-mid')
			plotstyle = 'lines'

		if plotstyle == 'errorbar':
			plot_raw[vis] = ax.errorbar(plot_data['x'], plot_data['y'], plot_data['ye'], xerr = plot_data['xe'],
				color = plot.get(color, 'k'), alpha = plot.get(alpha), label = plot.get(label),
				linewidth = plot.get('linewidth', 1),
				markersize = plot.get('markersize', 1), markevery = plot.get('markevery'),
				markerfacecolor = plot.get('markerfacecolor', plot.get(color)),
				fmt = plot.get('fmt', 'o'), capsize = plot.get('capsize', 0),
			)

		elif plotstyle == 'bars':
			plot_raw[vis] = ax.bar(plot_data['x'] - plot_data['xe'], plot_data['y'], 2 * plot_data['xe'],
				color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
				linewidth = plot.get('linewidth', 0),
			)

		elif plotstyle == 'lines':
			plot_raw[vis] = ax.plot(plot_data['x'], plot_data['y'], plot.get('fmt', 'o-'),
				color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
				linewidth=plot.get('linewidth', 1),
				markersize = plot.get('markersize', 1), markevery = plot.get('markevery'),
				markerfacecolor = plot.get('markerfacecolor', plot.get(color)),
				drawstyle = plot.get('drawstyle'),
			)

		elif (plotstyle == 'band') or (plotstyle == 'bandx') or (plotstyle == 'outline') or (plotstyle == 'bandline'):
			if plot.get('steps', False):
				y_low = plot_data['y'] - plot_data['ye'][0]
				y_high = plot_data['y'] + plot_data['ye'][1]
				y_low = numpy.ma.masked_invalid(y_low)
				y_high = numpy.ma.masked_invalid(y_high)
				xlims = (plot_data['x'] - plot_data['xe'], plot_data['x'] + plot_data['xe'])
				plot_data['x'] = numpy.dstack(xlims).flatten()
				y_low = numpy.repeat(y_low, 2)
				y_high = numpy.repeat(y_high, 2)
			else:
				y_low = plot_data['y'] - plot_data['ye'][0]
				y_high = plot_data['y'] + plot_data['ye'][1]

			if plotstyle == 'band':
				plot_raw[vis] = ax.fill_between(plot_data['x'], y_low, y_high,
					color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
					linewidth=plot.get('linewidth', 0)
				)
			if plotstyle == 'bandx':
				if plot_data['xe'].ndim == 2:
					x_low = plot_data['x'] - plot_data['xe'][0]
					x_high = plot_data['x'] + plot_data['xe'][1]
				else:
					x_low = plot_data['x'] - plot_data['xe']
					x_high = plot_data['x'] + plot_data['xe']
				x_low = numpy.ma.masked_invalid(x_low)
				x_high = numpy.ma.masked_invalid(x_high)
				plot_raw[vis] = ax.fill_betweenx(numpy.repeat(plot_data['y'], 2), x_low, x_high,
					color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
					linewidth=plot.get('linewidth', 0)
				)
			elif plotstyle == 'bandline':
				plot_raw[vis] = ax.fill_between(plot_data['x'], y_low, y_high,
					color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
					linewidth=plot.get('linewidth', 0)
				)
			elif plotstyle == 'outline':
				ax.plot(plot_data['x'], y_low, plot.get('fmt', ''),
					color = plot.get(color), alpha = plot.get(alpha),
					markersize = plot.get('markersize', 1), markevery = plot.get('markevery'),
					markerfacecolor = plot.get('markerfacecolor', plot.get(color)),
					linewidth=plot.get('linewidth', 1), drawstyle = plot.get('drawstyle')
				)
				plot_raw[vis] = ax.plot(plot_data['x'], y_high, plot.get('fmt', ''),
					color = plot.get(color), alpha = plot.get(alpha), label = plot.get(label),
					markersize = plot.get('markersize', 1), markevery = plot.get('markevery'),
					markerfacecolor = plot.get('markerfacecolor', plot.get(color)),
					linewidth=plot.get('linewidth', 1), drawstyle = plot.get('drawstyle')
				)
		else:
			raise


def drawPlots_old(ax, plots, opts = {}):
	for plot_raw in plots:
		plot = dict(opts)
		plot.update(plot_raw)
		if preset in plot:
			plot.update(plot_presets[plot[preset]])

		plotstyle = plot.get(style, 'errorbar')
		plotargs = {}
		def addOption(key, arg):
			if plot.get(key) != None:
				plotargs[arg] = plot.get(key)
		addOption(color, 'color')
		addOption(label, 'label')
		addOption(alpha, 'alpha')
		addOption('fmt', 'fmt')
		addOption('markersize', 'markersize')
		addOption('markevery', 'markevery')

		plot_data = {'x': plot[data]['x'], 'xe': plot[data]['xe']}
		def setEntry(src, target):
			if src:
				plot_data[target] = plot[data][src]
			else:
				plot_data[target] = numpy.zeros(len(plot[data]['x']))
		setEntry(plot.get('ysrc', 'y'), 'y')
		setEntry(plot.get('esrc', 'ye'), 'ye')

		def whiteMarker(plotargs):
			if plotargs['fmt'].startswith('#'):
				plotargs['markerfacecolor'] = 'w'
				plotargs['fmt'] = plotargs['fmt'].lstrip('#')

		if plotstyle == 'errorbar':
			plotargs.setdefault('fmt', 'o')
			whiteMarker(plotargs)
			markersize = plot.get('markersize', plotargs.get('markersize', 1))
			if 'markersize' in plotargs:
				plotargs.pop('markersize')
			plot_raw[vis] = ax.errorbar(plot_data['x'], plot_data['y'], plot_data['ye'], xerr=plot_data['xe'],
#				uplims=plot_data['ye'], lolims=plot_data['ye'],
				capsize=plot.get('capsize', 0), markersize=markersize, linewidth=plot.get('linewidth', 1), **plotargs)
		elif plotstyle == 'bars':
			plot_raw[vis] = ax.bar(plot_data['x'] - plot_data['xe'], plot_data['y'], 2 * plot_data['xe'],
				linewidth=plot.get('linewidth', 0), **plotargs)
		elif plotstyle == 'steps':
			whiteMarker(plotargs)
			fmt = ''
			if 'fmt' in plotargs:
				fmt = plotargs.pop('fmt')
			plot_raw[vis] = ax.plot(plot_data['x'], plot_data['y'], fmt,
				linewidth=plot.get('linewidth', 1), drawstyle='steps-mid', **plotargs)
		elif plotstyle == 'steps1':
			fmt = ''
			if 'fmt' in plotargs:
				fmt = plotargs.pop('fmt')
			plot_raw[vis] = ax.plot(plot_data['x'], plot_data['y'], fmt,
				linewidth=plot.get('linewidth', 1), drawstyle='steps-pre', **plotargs)
		elif plotstyle == 'lines':
			fmt = 'o-'
			if 'fmt' in plotargs:
				whiteMarker(plotargs)
				fmt = plotargs.pop('fmt')
			plot_raw[vis] = ax.plot(plot_data['x'], plot_data['y'], fmt,
				linewidth=plot.get('linewidth', 1), **plotargs)
		elif (plotstyle == 'band') or (plotstyle == 'outline'):
			if 'markersize' in plotargs:
				plotargs.pop('markersize')
			if ax.get_yscale() == 'log': # handle zero/subzero lower limits
				def maskZeroErrorEdge(data):
					return {'x': numpy.ma.masked_where(data['y'] <= data['ye'], data['x']), 'xe': data['xe'], 'y': data['y'], 'ye': data['ye']}
				tmp = maskZeroErrorEdge(plot_data)
			else:
				tmp = plot_data
			if ('yea' in tmp):
				y_low = tmp['y'] - tmp['yea'][0]
				y_high = tmp['y'] + tmp['yea'][1]
			elif (len(tmp['ye']) == 2):
				y_low = tmp['y'] - tmp['ye'][0]
				y_high = tmp['y'] + tmp['ye'][1]
			else:
				y_low = tmp['y'] - tmp['ye']
				y_high = tmp['y'] + tmp['ye']
			y_low = numpy.ma.masked_invalid(y_low)
			y_high = numpy.ma.masked_invalid(y_high)
			if plotstyle == 'band':
				plot_raw[vis] = ax.fill_between(tmp['x'], y_low, y_high,
					linewidth=plot.get('linewidth', 0), **plotargs)
			elif plotstyle == 'outline':
				if 'fmt' in plotargs:
					ds = None
					if 'drawstyle' in plotargs:
						ds = plotargs.pop('drawstyle')
					fmt = plotargs.pop('fmt')
					if fmt.startswith('#'):
						plotargs['markerfacecolor'] = 'w'
						fmt = fmt.lstrip('#')
					ax.plot(plot_data['x'], y_low, fmt,
						linewidth=plot.get('linewidth', 1), drawstyle=ds, **plotargs)
					plot_raw[vis] = ax.plot(plot_data['x'], y_high, fmt,
						linewidth=plot.get('linewidth', 1), drawstyle=ds, **plotargs)
				else:
					ds = 'steps-pre'
					if 'drawstyle' in plotargs:
						ds = plotargs.pop('drawstyle')
					ax.plot(plot_data['x'], y_low,
						linewidth=plot.get('linewidth', 1), drawstyle=ds, **plotargs)
					plot_raw[vis] = ax.plot(plot_data['x'], y_high,
						linewidth=plot.get('linewidth', 1), drawstyle=ds, **plotargs)
		else:
			raise


def drawArea(ax, xlim = None, ylim = None, color = 'gray', alpha = 0.2):
	def getLimits(usr, axis):
		if not usr:
			return axis
		(low, high) = usr
		if not low:
			low = axis[0]
		if not high:
			high = axis[1]
		return (low, high)
	xr = getLimits(xlim, ax.get_xlim())
	yr = getLimits(ylim, ax.get_ylim())
	ax.add_patch(matplotlib.patches.Rectangle((xr[0], yr[0]), xr[1] - xr[0], yr[1] - yr[0],
		color=color, linewidth=0, alpha=alpha))


def drawLines(ax, hlines = [], vlines = [], **kwargs):
	for hline in hlines:
		ax.hlines(hline, ax.get_xlim()[0], ax.get_xlim()[1], **kwargs)
	for vline in vlines:
		ax.vlines(vline, ax.get_ylim()[0], ax.get_ylim()[1], **kwargs)


def drawAnnotation(ax, notelist, opts = {}):
	for note in notelist:
		if isinstance(note, str):
			xp, yp, text = note.split(',', 2)
			note = {'x': xp, 'y': float(yp), label: text}
			if xp.startswith('r'):
				note.setdefault('ha', 'right')
				note['x'] = float(xp[1:])
			else:
				note['x'] = float(xp)
			note.setdefault('transform', 'axis')

		tf = ax.transAxes
		if note.get('transform') == 'data':
			tf = ax.transData
		ax.text(note.get('x', 0), note.get('y', 0), note[label],
			ha=note.get('ha', 'left'), va=note.get('va', 'center'), transform = tf, **opts)


def expFormat(x, y):
	tmp = tuple(map(float, ('%1.2e' % x).split('e')))
	if tmp[0] == 1.0:
		return '$10^{%d}$' % tmp[1]
	else:
		return '$%d\cdot 10^{%d}$' % tmp


def setupLogTicks(ax, major = [1,2,5]):
	ax.set_major_formatter(matplotlib.ticker.FuncFormatter(expFormat))
	minor = range(10)
	minor.remove(0)
	for x in major:
		minor.remove(x)
	ax.set_major_locator(matplotlib.ticker.LogLocator(10, major))
	ax.set_minor_locator(matplotlib.ticker.LogLocator(10, minor))


##################
# PLOT TEMPLATES

def doSimplePlot(fn, opts, plots, **kwargs):
	(fig, ax) = setupSimplePlot(opts, **kwargs)
	plots = filter(lambda p: p.get(data), plots)
	drawPlots(ax, plots, opts.get('plots', {}), opts.get('xy_switch', False))
	drawLegend(ax, plots, opts)
	if 'notesize' in opts:
		drawAnnotation(ax, opts.get(notes, []), {'fontsize': opts.get('notesize')})
	else:
		drawAnnotation(ax, opts.get(notes, []))
	lines = kwargs.get('lines', ([], []))
	drawLines(ax, lines[0], lines[1], **opts.get('lines', {}))
	if kwargs.get('fun'):
		kwargs.get('fun')(ax)
	savePlot(fig, fn)


def doCMSPlot(fn, opts, plots, **kwargs):
#	matplotlib.rc('font',**{'family':'Palatino','serif':'Palatino'})
	opts.setdefault('grid', True)
	(fig, ax) = setupCMSPlot(opts, **kwargs)
	ax.text(0, 1.02, r'CMS preliminary', transform=ax.transAxes, ha='left', va='bottom')
	infostr = []
	if opts.get('lumi'):
		infostr.append(r'$\mathcal{L} = %.1f\,\mathrm{fb}^{-1}$' % opts.get('lumi'))
	if opts.get('cms'):
		infostr.append(r'$\sqrt{s} = %s\,\mathrm{TeV}$' % opts.get('cms'))
	if infostr:
		ax.text(1, 1.02, str.join(' ', infostr), transform=ax.transAxes, ha='right', va='bottom')

	base_ax = ax

	plots_ax1 = filter(lambda p: p.get(data) and (p.get('axis', 1) == 1), plots)
	drawPlots(ax, plots_ax1, opts.get('plots', {}), opts.get('xy_switch', False))

	plots_ax2 = filter(lambda p: p.get(data) and (p.get('axis', 1) == 2), plots)
	if plots_ax2:
		xprefix = 'x'
		yprefix = 'y'
		if 'x2range' in opts:
			xprefix = 'x2'
			base_ax = base_ax.twiny()
		if 'y2range' in opts:
			yprefix = 'y2'
			base_ax = base_ax.twinx()
		setupAxis(base_ax, opts, xprefix = xprefix, yprefix = yprefix)
		drawPlots(base_ax, plots_ax2, opts.get('plots2', opts.get('plots', {})), opts.get('xy_switch', False))

	drawLegend(base_ax, plots, opts)
	if 'notesize' in opts:
		drawAnnotation(base_ax, opts.get(notes, []), {'fontsize': opts.get('notesize')})
	else:
		drawAnnotation(base_ax, opts.get(notes, []))
	if 'area' in opts:
		drawArea(base_ax, xlim = opts['area'].get('xlim'), ylim = opts['area'].get('ylim'))
	lines = kwargs.get('lines', ([], []))
	drawLines(base_ax, lines[0], lines[1], **opts.get('lines', {}))
	if kwargs.get('fun'):
		kwargs.get('fun')(base_ax)
	savePlot(fig, fn)


def doSimpleRatioPlot(fn, opts, plots, rplots, **kwargs):
	(fig, ax, rax) = setupRatioPlot(opts, opts.get('xrange'), opts.get('yrange'), opts.get('rrange'), **kwargs)
	plots = filter(lambda p: p.get(data), plots)
	drawPlots(ax, plots, opts.get('plots', {}), opts.get('xy_switch', False))
	drawLegend(ax, plots, opts)
	if 'notesize' in opts:
		drawAnnotation(ax, opts.get(notes, []), {'fontsize': opts.get('notesize')})
	else:
		drawAnnotation(ax, opts.get(notes, []))
	if opts.get('x1hide', True):
		ax.xaxis.set_major_formatter(matplotlib.ticker.NullFormatter())
		ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
	if kwargs.get('fun'):
		kwargs.get('fun')(ax)
	rplots = filter(lambda p: p.get(data), rplots)
	drawPlots(rax, rplots, opts.get('plots', {}), opts.get('xy_switch', False))
	drawLegend_int(rax, rplots, **opts.get('rlegend', {}))
	drawLines(rax, [1])
	if kwargs.get('rfun'):
		kwargs.get('rfun')(ax)
	savePlot(fig, fn)


def doSimple2DPlot(fn, opts, plot2d, plots = [], **kwargs):
	for key in ['range', 'label', 'scale', 'prec']:
		if key in opts:
			if ('x' + key) not in opts:
				opts['x' + key] = opts[key]
			if ('y' + key) not in opts:
				opts['y' + key] = opts[key]

	if opts.get('theme') == 'cms':
		(fig, ax) = setupCMSPlot(opts, **kwargs)
		ax.text(0, 1.02, r'CMS preliminary', transform=ax.transAxes, ha='left', va='bottom')
		infostr = opts.get('infostr', [])
		if opts.get('lumi'):
			infostr.append(r'$\mathcal{L} = %.1f\,\mathrm{fb}^{-1}$' % opts.get('lumi'))
		if opts.get('cms'):
			infostr.append(r'$\sqrt{s} = %s\,\mathrm{TeV}$' % opts.get('cms'))
		if infostr:
			ax.text(1, 1.02, str.join(' ', infostr), transform=ax.transAxes, ha='right', va='bottom')
	else:
		(fig, ax) = setupSimplePlot(opts, **kwargs)

	z_values = numpy.array(plot2d['z'])
	zs = opts.get('zscale', 'linear')
	if zs == 'log':
		cm = opts.get('cmap', 'jet')
		z_values = numpy.ma.masked_where(z_values <= 0, z_values)
		zr = opts.get('zrange', (z_values.min(), z_values.max()))
		norm = matplotlib.colors.LogNorm(vmin=zr[0], vmax=zr[1], clip = opts.get('clip', False))
	else:
		zr = opts.get('zrange', (-1, 1))
		cm = opts.get('cmap', 'seismic')
		norm = matplotlib.colors.Normalize(zr[0], zr[1], clip = opts.get('clip', False))
	z_values = numpy.ma.masked_invalid(z_values)

	if opts.get('2dmode', 'mesh') == 'mesh':
		mesh = ax.pcolormesh(plot2d['x'], plot2d['y'], z_values, cmap = plt.get_cmap(cm), alpha = 1, norm = norm)
	else:
		mesh = ax.pcolor(plot2d['x'], plot2d['y'], z_values, cmap = plt.get_cmap(cm), alpha = 1, norm = norm)
	cb = fig.colorbar(mesh, ax=ax, aspect=20, fraction = 0.1, pad = 0.01, format='$%.1f$')

	def cbExpFormat(x, y):
		x = (zr[0]*(zr[1]/zr[0])**x)
		tmp = tuple(map(float, ('%1.2e' % x).split('e')))
		if tmp[0] == 1.0:
			return '$10^{%d}$' % tmp[1]
		else:
			return '$%d\cdot 10^{%d}$' % tmp
	if zs == 'log':
		cb.ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(cbExpFormat))
	cb.set_label(opts.get('zlabel', ''))

	drawPlots(ax, plots, opts.get('xy_switch', False))
	ax.grid(opts.get('grid', False))

	if 'notesize' in opts:
		drawAnnotation(ax, opts.get(notes, []), {'fontsize': opts.get('notesize')})
	else:
		drawAnnotation(ax, opts.get(notes, []))

	drawLegend(ax, plots, opts)
#	drawLines(ax, [1])

	savePlot(fig, fn)


def doSimpleCorrelationPlot(fn, opts, src, showErrors = True, covkey = 'cov', showCov = False):
	if showErrors:
		fig = plt.figure(figsize=(6, 6))
		ax1 = fig.add_axes((0.15, 0.3, 0.72, 0.65), xlim = opts.get('xrange'), ylim = opts.get('yrange1')) # l,b,w,h
	else:
		fig = plt.figure(figsize=(6, 5))
		ax1 = fig.add_axes((0.15, 0.1, 0.72, 0.85), xlim = opts.get('xrange'), ylim = opts.get('yrange1')) # l,b,w,h
	opts['yscale'] = opts.get('yscale1', 'linear')
	opts['ylabel'] = opts.get('ylabel1', 'y1')
	setupAxis(ax1, opts)
	ax1.xaxis.set_major_formatter(matplotlib.ticker.NullFormatter())
	ax1.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())

	if opts.get('theme') == 'cms':
		ax1.text(0, 1.02, r'CMS preliminary', transform=ax1.transAxes, ha='left', va='bottom')
		infostr = []
		if opts.get('lumi'):
			infostr.append(r'$\mathcal{L} = %.1f\,\mathrm{fb}^{-1}$' % opts.get('lumi'))
		if opts.get('cms'):
			infostr.append(r'$\sqrt{s} = %s\,\mathrm{TeV}$' % opts.get('cms'))
		if infostr:
			ax1.text(1, 1.02, str.join(' ', infostr), transform=ax1.transAxes, ha='right', va='bottom')

	x = numpy.array(list(src['x'] - src['xe']) + [list(src['x'])[-1] + src['xe'][-1]])
	ye_err = numpy.sqrt(numpy.diagonal(src[covkey]))
	if showCov:
		z = numpy.ma.masked_less(src[covkey], 1e-20)
		norm = matplotlib.colors.LogNorm(z.min(), z.max())
		zlabel = r'$\mathrm{Covariance}$'
	else:
		z = src[covkey] / numpy.outer(ye_err, ye_err)
		norm = matplotlib.colors.Normalize(-1, 1)
		zlabel = r'$\mathrm{Correlation}$'
	mesh = ax1.pcolormesh(x, x, z, cmap = plt.get_cmap('bwr'), alpha = 1,norm = norm)
	cb = fig.colorbar(mesh, ax=ax1, aspect=10, fraction = 0.05, pad = 0.02, format='$%.1f$')
	cb.set_label(zlabel)

	if showErrors:
		ax2 = fig.add_axes((0.15, 0.1, 0.72*(1-0.05-0.02), 0.2), xlim = opts.get('xrange'), ylim = opts.get('yrange2'))
		opts['yscale'] = opts.get('yscale2', 'linear')
		setupAxis(ax2, opts)
		ax2.yaxis.set_major_formatter(matplotlib.ticker.FormatStrFormatter('$%.1f$'))
		ax2.set_ylabel(opts.get('ylabel2', 'y'), va = 'center', y = 0.5)
		tmp = dict(src)
		tmp['ye'] = ye_err
		plots = [{data: scaleData(getRelativeError(tmp, yoffset=0), 100), label: 'Error', style: 'band', alpha: 0.5, color: 'k'}]
		drawPlots(ax2, plots, xy_switch = opts.get('xy_switch', False))
		drawLines(ax2, [0])
	savePlot(fig, fn)
