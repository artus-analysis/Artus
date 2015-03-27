# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class ScaleByIntegral(analysisbase.AnalysisBase):
	"""Scale histograms by (inverse) integral of other histogram."""

	def modify_argument_parser(self, parser, args):
		super(ScaleByIntegral, self).modify_argument_parser(parser, args)

		self.scale_by_integral_options = parser.add_argument_group("{} options".format(self.name()))
		self.scale_by_integral_options.add_argument(
				"--histogram-to-scale-nicks", type=str, nargs="+",
				help="Nick names for the histograms to be scaled (in place)."
		)
		self.scale_by_integral_options.add_argument(
				"--integral-histogram-nicks", type=str, nargs="+",
				help="Nick names for the histogram to calculate the integral from."
		)
		self.scale_by_integral_options.add_argument(
				"--scale-by-inverse-integrals", type="bool", nargs="+", default=[True],
				help="Scale histogram by inverse integrals. [Default: %(default)s]"
		)

	def prepare_args(self, parser, plotData):
		super(ScaleByIntegral, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["histogram_to_scale_nicks", "integral_histogram_nicks", "scale_by_inverse_integrals"])

	def run(self, plotData=None):
		super(ScaleByIntegral, self).run(plotData)

		for histogram_to_scale_nick, integral_histogram_nick, scale_by_inverse_integrals in zip(
				*[plotData.plotdict[k] for k in ["histogram_to_scale_nicks", "integral_histogram_nicks", "scale_by_inverse_integrals"]]
		):
			histogram_to_scale = plotData.plotdict["root_objects"][histogram_to_scale_nick]
			integral_histogram = plotData.plotdict["root_objects"][integral_histogram_nick]
			assert(isinstance(histogram_to_scale, ROOT.TH1))
			assert(isinstance(integral_histogram, ROOT.TH1))
			histogram_to_scale.Scale((1.0 / integral_histogram.Integral()) if scale_by_inverse_integrals else integral_histogram.Integral())
			plotData.plotdict["root_objects"][histogram_to_scale_nick] = histogram_to_scale

