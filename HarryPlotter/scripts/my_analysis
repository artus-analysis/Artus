#!/usr/bin/env python
# -*- coding: utf-8 -*-

import Artus.HarryPlotter.harrycore as harrycore

# import analysis specific functions
import myanalysis_plots
import subprocess

if __name__ == "__main__":
	"""This is a template for users to make analysis-specific plots with HarryPlotter."""

	basedict = harrycore.get_basic_dictionary()

	# Add analysis-specific default values:
	analysisdict = {'title': 'own work'}
	basedict.update(analysisdict)

	parser = harrycore.get_basic_parser(**basedict)

	# Add custom parser arguments
	parser.add_argument('--websync', action='store_true',
	help="Copy the plot to your public EKP webspace")
	parser.add_argument('--scalefactor', type=float, default=1.,
		help="add a custom scalefactor. Default is %(default)s")

	plotdict = harrycore.create_dictionary_from_parser(parser)

	# Add the module with the user-written functions to the plotdict
	plotdict['analysismodules'] =[myanalysis_plots]

	# Map the samples to weights from a dictionary:
	for i, label in enumerate(plotdict['labels']):
		if label in myanalysis_plots.sampleweightdict:
			plotdict['weight'] += "* %s" % myanalysis_plots.sampleweightdict['label']
		else:
			print "Sample %s could not be found in dictionary!" % label

	harrycore.plot(plotdict)

	# perform custom action after plot creation
	if plotdict['websync']:
		filename =  "%s/%s.%s" % (plotdict['out'], plotdict['filename'], plotdict['formats'][0])
		subprocess.call(['rsync', filename, 'ekplx26:/disks/ekpwww/web/${USER}/public_html/webplot.%s' % plotdict['formats'][0]])


