# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysisbase as analysisbase

class ScaleBinContents(analysisbase.AnalysisBase):
	"""Scale selected root histogram(s) by a certain value."""
	"""
	In contrast to the scaling function of the InputRoot module, this module
	can e.g. be applied to histograms that are created by other analysis modules.
	"""

	def modify_argument_parser(self, parser, args):
		super(ScaleBinContents, self).modify_argument_parser(parser, args)

		self.ScaleBinContents_options = parser.add_argument_group("ScaleBinContents options")
		self.ScaleBinContents_options.add_argument("--scale-bin-contents", type=str, nargs='*', default=[],
				help="List the nicks you want to scale (-> module whitelist ).")
		self.ScaleBinContents_options.add_argument("--scale", type=float, default=0.,
				help="Value by which you want the histogram to be scaleed.")

	def run(self, plotData=None):
		super(ScaleBinContents, self).run(plotData)

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if nick in plotData.plotdict['scale_bin_contents']:
				root_histogram.Scale(plotData.plotdict['scale'])
