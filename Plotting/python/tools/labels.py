# -*- coding: utf-8 -*-

"""This module contains functions to add various labels to the plot."""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import time


def add_labels(plotdict):
	"""This function prints all labels and captions in a plot.

	Several functions are called for each type of label.
	"""
	add_lumilabel(plotdict)
	add_energylabel(plotdict)
	add_authorlabel(plotdict)
	add_axislabels(plotdict)
	add_datelabel(plotdict)
	add_eventnumberlabel(plotdict)
	add_textlabel(plotdict)
	if plotdict['legloc'] not in [None, 'None']:
		plotdict['axes'].legend(loc=plotdict['legloc'], numpoints=1, fancybox=True, shadow=True)


def add_textlabel(plotdict, x=0.02, y=0.02):
	if plotdict['text']:
		if len(plotdict['text'].split(",")) % 3 == 0:
			texts = plotdict['text'].split(",")
		else:
			texts = [plotdict['text'], x, y]
		for text, x, y in zip(texts[::3], texts[1::3], texts[2::3]):
			plotdict['axes'].text(x, y, text, va='bottom', ha='left', transform=plotdict['axes'].transAxes,
					size='small', color='black')

def add_eventnumberlabel(plotdict):
	if plotdict['eventnumberlabel']:
		if len(plotdict['mplhistos']) > 0:
			text = ""
			for histo, label in zip(plotdict['mplhistos'], plotdict['labels']):
				text += "\n %s Events: %1.1e" % (label, histo.ysum())
			plotdict['axes'].text(0.7, 1.01, text, size='xx-small', va='bottom', ha='right',
					transform=plotdict['axes'].transAxes)


def add_lumilabel(plotdict, xpos=0.00, ypos=1.01):
	if plotdict['lumi']:
		if plotdict['lumi'] >= 1.0:
			plotdict['axes'].text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{fb}^{-1}$" %
				(plotdict['lumi']), va='bottom', ha='left', transform=plotdict['axes'].transAxes)
		elif plotdict['lumi'] > 0.0:
			plotdict['axes'].text(xpos, ypos, r"$\mathcal{L} = %1.1f\,\mathrm{pb}^{-1}$" %
				(plotdict['lumi'] * 1000.0), va='bottom', ha='left', transform=plotdict['axes'].transAxes)


def add_energylabel(plotdict, xpos=1.00, ypos=1.01):
	if plotdict['energy']:
		plotdict['axes'].text(xpos, ypos, r"$\sqrt{s} = %u\,\mathrm{TeV}$" % (plotdict['energy']),
			va='bottom', ha='right', transform=plotdict['axes'].transAxes)



def add_authorlabel(plotdict, xpos=0.0, ypos=1.10):
	if plotdict['author'] is not None:
		plotdict['axes'].text(xpos, ypos, plotdict['author'], va='top', ha='left',
				transform=plotdict['axes'].transAxes)


def add_datelabel(plotdict, xpos=1.0, ypos=1.10):
	if plotdict['date'] is None:
		return
	if plotdict['date'] == 'now':
		plotdict['axes'].text(xpos, ypos, time.strftime("%d %b %Y %H:%M", time.localtime()),
		va='top', ha='right', transform=plotdict['axes'].transAxes)
	elif plotdict['date'] == 'today':
		plotdict['axes'].text(xpos, ypos, time.strftime("%d %b %Y", time.localtime()),
		va='top', ha='right', transform=plotdict['axes'].transAxes)
	elif plotdict['date'] == 'iso':
		plotdict['axes'].text(xpos, ypos, time.strftime("%Y-%m-%d", time.localtime()),
		va='top', ha='right', transform=plotdict['axes'].transAxes)
	else:
		plotdict['axes'].text(xpos, ypos, plotdict['date'], va='top', ha='right', transform=plotdict['axes'].transAxes)


def add_axislabels(plotdict):
	"""Set axis labels and limits"""

	if 'ratiosubplotaxes' in plotdict:
		xax = plotdict['ratiosubplotaxes']
	else:
		xax = plotdict['axes']
	# labels
	xax.set_xlabel(plotdict['xname'], ha="right", x=1)
	if plotdict['yname']:
		plotdict['axes'].set_ylabel(plotdict['yname'], va="top", y=1)

	# axis tick marks
	if plotdict['xticks']:
		xax.set_xticks(plotdict['xticks'])
		xax.set_xticklabels([str(x) for x in plotdict['xticks']])
	if plotdict['yticks']: plotdict['axes'].set_yticks(plotdict['yticks'])



