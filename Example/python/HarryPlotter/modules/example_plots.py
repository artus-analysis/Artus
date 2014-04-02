# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.harrycore as harrycore
import Artus.HarryPlotter.plot1d as plot1d
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.tools.mplconvert as mplconvert


sampleweightdict = {
	'signal': 35.5,
	'background': 234.5,
}

def example_plot1(plotdict):
	""" Template: place custom plot code in a function like this."""
	log.info("Hallo")


def example_plotscale(plotdict):
	""" This function does basically the same as plot1d, but scales the root
		histograms with a factor and then convertes them to MPL.
	"""
	#TODO: The conversion ROOT->MPL is actually done twice - avoid this

	plot1d.get_root_histos(plotdict)

	for n, (roothisto, rootfile) in enumerate(zip(plotdict['roothistos'], plotdict['rootfiles'])):
		# Scale (or whatever you want to do with the root histos):
		roothisto.Scale(plotdict['scalefactor'])
		plotdict["mplhistos"][n] = mplconvert.root2histo(roothisto, rootfile.GetName(), 1)

	plot1d.plot1d_mpl(plotdict)
	utils.setaxislimits(plotdict)
	labels.add_labels(plotdict)
	utils.save(plotdict)
