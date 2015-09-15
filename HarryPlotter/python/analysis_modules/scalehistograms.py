# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools

class ScaleHistograms(analysisbase.AnalysisBase):
	"""Scale selected root histogram(s) by a certain value."""
	"""
	In contrast to the scaling function of the InputRoot module, this module
	can e.g. be applied to histograms that are created by other analysis modules.
	"""

	def modify_argument_parser(self, parser, args):
		super(ScaleHistograms, self).modify_argument_parser(parser, args)

		self.ScaleHistograms_options = parser.add_argument_group(self.name() +" options")
		self.ScaleHistograms_options.add_argument("--scale-nicks", type=str, nargs='*', default=[],
				help="List the nicks you want to scale (-> module whitelist ).")
		self.ScaleHistograms_options.add_argument("--scale", type=float, default=1.,
				help="Value by which you want the histogram to be scaled.")

	def run(self, plotData=None):
		super(ScaleHistograms, self).run(plotData)

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if nick in plotData.plotdict['scale_nicks']:
				log.debug("Scale nick '{0}' by {1}".format(nick, plotData.plotdict['scale']))
				if isinstance(root_histogram, ROOT.TGraph):
					roottools.RootTools.scale_tgraph(root_histogram, plotData.plotdict['scale'])
				else:
					root_histogram.Scale(plotData.plotdict['scale'])
