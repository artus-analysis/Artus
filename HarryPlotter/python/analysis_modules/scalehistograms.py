# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools

class ScaleHistograms(analysisbase.AnalysisBase):
	"""Scale selected root histogram(s) by certain values."""
	"""
	In contrast to the scaling function of the InputRoot module, this module
	can e.g. be applied to histograms that are created by other analysis modules.
	"""

	def modify_argument_parser(self, parser, args):
		super(ScaleHistograms, self).modify_argument_parser(parser, args)

		self.ScaleHistograms_options = parser.add_argument_group(self.name() +" options")
		self.ScaleHistograms_options.add_argument("--scale-nicks", type=str, nargs='*', default=[],
				help="List the nicks you want to scale (-> module whitelist ).")
		self.ScaleHistograms_options.add_argument("--scales", type=float, default=[1.], nargs='*', 
				help="Values by which you want the histogram to be scaled.")
		self.ScaleHistograms_options.add_argument("--scale-result-nicks", type=str, default=None,
				help="Nicks of the resulting histograms. The default behaviour is that the same nick is kept.")

	def prepare_args(self, parser, plotData):
		super(ScaleHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["scale_nicks", "scales", "scale_result_nicks"])

	def run(self, plotData=None):
		super(ScaleHistograms, self).run(plotData)

		for scale_nick, scale, result_nick in zip( plotData.plotdict['scale_nicks'],
			                                plotData.plotdict['scales'],
			                                plotData.plotdict['scale_result_nicks'] ):
			root_histogram = plotData.plotdict["root_objects"][scale_nick]
			if result_nick != None:
				plotData.plotdict["root_objects"][result_nick] = root_histogram.Clone(result_nick)
				root_histogram = plotData.plotdict["root_objects"][result_nick]
			log.debug("Scale nick '{0}' by {1}".format(scale_nick, scale))
			if isinstance(root_histogram, ROOT.TGraph):
				roottools.RootTools.scale_tgraph(root_histogram, scale)
			else:
				root_histogram.Scale(scale)
