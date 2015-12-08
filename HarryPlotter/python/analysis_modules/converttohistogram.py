# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.roottools as roottools


class ConvertToHistogram(analysisbase.AnalysisBase):
	"""Convert root objects (TProfiles, TGraphs) to histograms."""

	def modify_argument_parser(self, parser, args):
		super(ConvertToHistogram, self).modify_argument_parser(parser, args)
		self.tohistogram_options = parser.add_argument_group("{} options".format(self.name()))
		self.tohistogram_options.add_argument("--convert-nicks", type=str, nargs="+",
				help="Nicks to be converted. If not specified, all nicks are used.")

	def prepare_args(self, parser, plotData):
		super(ConvertToHistogram, self).prepare_args(parser, plotData)
		if plotData.plotdict['convert_nicks'] is None:
			plotData.plotdict['convert_nicks'] = plotData.plotdict['nicks']

	def run(self, plotData=None):
		super(ConvertToHistogram, self).run(plotData)
		for nick in plotData.plotdict['convert_nicks']:
			plotData.plotdict['root_objects'][nick] = roottools.RootTools.to_histogram(plotData.plotdict['root_objects'][nick])

