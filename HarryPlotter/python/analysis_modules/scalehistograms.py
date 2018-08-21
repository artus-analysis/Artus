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
		self.ScaleHistograms_options.add_argument("--divide-by-scales", action="store_true", default=[False], 
		help="If selected the given value for the scale will be used to divide by the given scale factor.")
		self.ScaleHistograms_options.add_argument("--divide-by-qcd-osss-factors", action="store_true", default=[False], 
		help="If selected the given value for the scale will be one over the qcd_osss_factor given by the nicks.")	
		self.ScaleHistograms_options.add_argument("--divide-by-qcd-and-wj-factors", action="store_true", default=[False], 
		help="If selected the given value for the scale will be one over the qcd_osss_factor given by the nicks.")				
		self.ScaleHistograms_options.add_argument("--scale-result-nicks", type=str, default=None,
				help="Nicks of the resulting histograms. The default behaviour is that the same nick is kept.")

	def prepare_args(self, parser, plotData):
		super(ScaleHistograms, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["scale_nicks", "scales", "scale_result_nicks", "divide_by_scales", "divide_by_qcd_osss_factors", "divide_by_qcd_and_wj_factors"])

	def run(self, plotData=None):
		super(ScaleHistograms, self).run(plotData)
		
		for scale_nick, scale, result_nick, divide_by_scale, divide_by_qcd_osss_factor, divide_by_qcd_and_wj_factor in zip( plotData.plotdict['scale_nicks'],
			                                plotData.plotdict['scales'],
			                                plotData.plotdict['scale_result_nicks'],
			                                plotData.plotdict['divide_by_scales'],
											plotData.plotdict['divide_by_qcd_osss_factors'],
											plotData.plotdict['divide_by_qcd_and_wj_factors']):
			root_histogram = plotData.plotdict["root_objects"][scale_nick]
			if (divide_by_scale == True and not divide_by_qcd_osss_factor == True):
				scale = 1./scale 
				log.debug("Divide by scale {scale}Scaling nick with name {nick} by {scale}".format(nick=scale_nick, scale=scale))
			if (divide_by_qcd_osss_factor == True and "qcd" in scale_nick):
				scale = 1./plotData.plotdict['qcd_extrapolation_factors_ss_os'][0]
				log.debug("Scaling nick with name {nick} by {scale}".format(nick=scale_nick, scale=scale))
			if result_nick != None:
				plotData.plotdict["root_objects"][result_nick] = root_histogram.Clone(result_nick)
				root_histogram = plotData.plotdict["root_objects"][result_nick]
			log.debug("Scale nick '{0}' by {1}".format(scale_nick, scale))
			if isinstance(root_histogram, ROOT.TGraph):
				roottools.RootTools.scale_tgraph(root_histogram, scale)
			else:
				root_histogram.Scale(scale)
