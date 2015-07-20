# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysisbase as analysisbase

class HistogramManipulationBase(analysisbase.AnalysisBase):
	"""Base class for histogram-manipulation processors."""

	def prepare_args(self, parser, plotData):
		super(HistogramManipulationBase, self).prepare_args(parser, plotData)
		self.whitelist = []

	def run(self, plotData=None):
		super(HistogramManipulationBase, self).run(plotData)

		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if self._selector(nick, root_histogram, plotData):
				for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
					for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
						for z_bin in xrange(1, root_histogram.GetNbinsZ()+1):
							global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
							self._manipulate_bin(root_histogram, global_bin)

	def _selector(self, nick, root_histogram, plotData):
		"""This function evaluates conditions that nicks / histograms have to pass."""
		if not hasattr(self, 'whitelist') or nick in self.whitelist:
			log.debug(self.name() + ": Now editing nick %s" % nick)
			return True
		else:
			return False

	def _manipulate_bin(self, histogram, global_bin):
		pass
