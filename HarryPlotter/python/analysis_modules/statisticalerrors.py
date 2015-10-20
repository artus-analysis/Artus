# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class StatisticalErrors(analysisbase.AnalysisBase):
	"""Replace bins contents of histograms by absolute errors (bin error) or relative errors (bin error / bin content).\n
	   Replace error bars of graphs by absolute errors (error) or relative errors (error / central value) and set the central value to 0."""
	
	def __init__(self):
		super(StatisticalErrors, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(StatisticalErrors, self).modify_argument_parser(parser, args)

		self.StatisticalErrors_options = parser.add_argument_group("StatisticalErrors options")
		self.StatisticalErrors_options.add_argument("--stat-error-nicks", type=str, nargs="+",
				help="Nick names of the histograms/graphs to be modified in place.")
		self.StatisticalErrors_options.add_argument("--stat-error-relative", nargs="?", type="bool", default=False, const=True,
				help="Do not plot absolute errors, but relative to bin content. [Default: %(default)s]")
		self.StatisticalErrors_options.add_argument("--stat-error-relative-percent", nargs="?", type="bool", default=False, const=True,
				help="Output relative errors as percentage. [Default: %(default)s]")
	
	def prepare_args(self, parser, plotData):
		super(StatisticalErrors, self).prepare_args(parser, plotData)
		if plotData.plotdict["stat_error_nicks"] is None:
			plotData.plotdict["stat_error_nicks"] = plotData.plotdict["nicks"]
	
	def run(self, plotData=None):
		super(StatisticalErrors, self).run(plotData)

		for nick in plotData.plotdict["stat_error_nicks"]:
			root_object = plotData.plotdict["root_objects"][nick]
			
			if isinstance(root_object, ROOT.TH1) and not isinstance(root_object, ROOT.TProfile):
				for x_bin in xrange(1, root_object.GetNbinsX()+1):
					for y_bin in xrange(1, root_object.GetNbinsY()+1):
						for z_bin in xrange(1, root_object.GetNbinsZ()+1):
							global_bin = root_object.GetBin(x_bin, y_bin, z_bin)
							root_object.SetBinContent(global_bin, StatisticalErrors.rel_error(
									root_object.GetBinContent(global_bin),
									root_object.GetBinError(global_bin),
									relative=plotData.plotdict["stat_error_relative"],
									percent=plotData.plotdict["stat_error_relative_percent"]
							))
							root_object.SetBinError(global_bin, 0.0)
			
			elif isinstance(root_object, ROOT.TGraph) and (not isinstance(root_object, ROOT.TGraph2D)):
				has_errors = isinstance(root_object, ROOT.TGraphErrors)
				has_asymm_errors = isinstance(root_object, ROOT.TGraphAsymmErrors)
				for point in xrange(root_object.GetN()):
					x_value, y_value = array.array("d", [0.0]), array.array("d", [0.0])
					root_object.GetPoint(point, x_value, y_value)
					x_value = x_value[0]
					y_value = y_value[0]
					
					x_error_high = root_object.GetErrorXhigh(point)
					x_error_low = root_object.GetErrorXlow(point)
					y_error_high = root_object.GetErrorYhigh(point)
					y_error_low = root_object.GetErrorYlow(point)
					
					y_rel_error_high = StatisticalErrors.rel_error(
							y_value,
							y_error_high,
							relative=plotData.plotdict["stat_error_relative"],
							percent=plotData.plotdict["stat_error_relative_percent"]
					)
					y_rel_error_low = StatisticalErrors.rel_error(
							y_value,
							y_error_low,
							relative=plotData.plotdict["stat_error_relative"],
							percent=plotData.plotdict["stat_error_relative_percent"]
					)
					
					root_object.SetPoint(point, x_value, 0.0)
					if has_asymm_errors:
						root_object.SetPointError(point, x_error_low, x_error_high, y_rel_error_low, y_rel_error_high)
					elif has_errors:
						root_object.SetPointError(point, x_error_high, y_rel_error_high)
			
			else:
				log.warning("Module \"{name}\" does not support object of type \"{object_type}\" (nick \"{nick}\"".format(
						name=self.name(),
						object_type=str(type(root_object)),
						nick=nick
				))
	
	@staticmethod
	def rel_error(central, error, relative=False, percent=False):
		result = error
		if relative and (central != 0.0):
			result /= central
		return (result*100. if percent else result)

