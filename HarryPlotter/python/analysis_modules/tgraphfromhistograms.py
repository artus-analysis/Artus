# -*- coding: utf-8 -*-

import logging
from collections import deque
import sys
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class HistogramGroupIterator(object):
	SKIP_NONE = 0
	SKIP_ANY = 1
	SKIP_LEADING = 2
	SKIP_TRAILING = 4
	SKIP_BORDERING = SKIP_LEADING | SKIP_TRAILING
	def __init__(self, histograms, skip=False):
		"""
		Iterator for groups of histograms

		This class iterates bin-wise in X over all its histograms. For each bin,
		a list of `[(h1, h1err), (h2, h2err), ...]` is yielded.
		"""
		if len(set(histo.GetNbinsX() for histo in histograms)) > 1:
			raise ValueError("Histograms must be of same bin count in X")
		self.histograms = histograms
		self.skip = skip

	def _histo_group_iter(self):
		"""
		Actual iteration over histogram values without skipping

		:returns: iterator of each histogram's content and error at each bin
		:rtype: collections.Iterable[list[tuple(float, float)]]
		"""
		for i in range(1, self.histograms[0].GetNbinsX()+1):
			yield [(histo.GetBinContent(i), histo.GetBinError(i)) for histo in self.histograms]

	def __iter__(self):
		raw_iter = self._histo_group_iter()
		if self.skip == self.SKIP_ANY:
			return self._iter_skip_any_(raw_iter)
		if self.skip & self.SKIP_LEADING:
			raw_iter = self._iter_skip_leading(raw_iter)
		if self.skip & self.SKIP_TRAILING:
			raw_iter = self._iter_skip_trailing(raw_iter)
		return raw_iter

	@staticmethod
	def _iter_skip_any_(iterable):
		"""Iterator skipping any bin that is empty in all histograms"""
		for bin_contents in iterable:
			# yield if any indivial histo has any non-empty content
			if any(any(histo_content) for histo_content in bin_contents):
				yield bin_contents

	@staticmethod
	def _iter_skip_leading(iterable):
		"""Iterator skipping leading bins that are empty in all histograms"""
		leading = True
		for bin_contents in iterable:
			# start yield once any indivial histo has any non-empty content
			if leading:
				if any(any(histo_content) for histo_content in bin_contents):
					leading = False
					yield bin_contents
			if not leading:
				yield bin_contents

	@staticmethod
	def _iter_skip_trailing(iterable):
		"""Iterator skipping trailing bins that are empty in all histograms"""
		trail_buffer = deque()
		for bin_contents in iterable:
			# buffer any empty ranges for later yield if sequence resumes
			if any(any(histo_content) for histo_content in bin_contents):
				for elem in trail_buffer:
					yield elem
				yield bin_contents
			else:
				trail_buffer.append(bin_contents)


class TGraphFromHistograms(analysisbase.AnalysisBase):
	"""This module takes two (or more) histograms and uses their y-values (and errors) as x,y-pairs for a TGraphErrors."""
	def modify_argument_parser(self, parser, args):
		super(TGraphFromHistograms, self).modify_argument_parser(parser, args)

		self.tgraph_options = parser.add_argument_group("{} options".format(self.__class__.__name__))
		self.tgraph_options.add_argument("--tgraph-x-nicks", type=str, nargs="+",
			help="Nick names for histograms with the x-values of the graph points.")
		self.tgraph_options.add_argument("--tgraph-y-nicks", type=str, nargs="+",
			help="Nick names for histograms with the y-values of the graph points.")
		self.tgraph_options.add_argument("--tgraph-result-nicks", type=str, nargs="+",
			help="Nick names for the resulting TGraphErrors.")
		self.tgraph_options.add_argument("--tgraph-strip-empty", type=str, nargs="?", const=True,
			help="Skip bins that are empty in both histograms. Without parameter, skip leading and trailing."
				 " With 'any', skip all. With 'left' or 'right' skip leading or trailing, respectively.")

	def prepare_args(self, parser, plotData):
		super(TGraphFromHistograms, self).prepare_args(parser, plotData)
		plot_dict = plotData.plotdict
		if plot_dict.get('tgraph_strip_empty'):
			if plot_dict['tgraph_strip_empty'] == True:
				plot_dict['tgraph_strip_empty'] = HistogramGroupIterator.SKIP_BORDERING
			elif isinstance(plot_dict['tgraph_strip_empty'], basestring):
				if plot_dict['tgraph_strip_empty'].lower() == 'any':
					plot_dict['tgraph_strip_empty'] = HistogramGroupIterator.SKIP_ANY
				elif plot_dict['tgraph_strip_empty'].lower() == 'left':
					plot_dict['tgraph_strip_empty'] = HistogramGroupIterator.SKIP_LEADING
				elif plot_dict['tgraph_strip_empty'].lower() == 'right':
					plot_dict['tgraph_strip_empty'] = HistogramGroupIterator.SKIP_TRAILING
				else:
					raise ValueError("Unrecognized option for 'tgraph_strip_empty': %s" % plotData['tgraph_strip_empty'])
		else:
			plot_dict['tgraph_strip_empty'] = HistogramGroupIterator.SKIP_NONE
		self.prepare_list_args(plotData, ["tgraph_x_nicks", "tgraph_y_nicks", "tgraph_result_nicks"])
		super(TGraphFromHistograms, self).auto_set_arguments(plotData, ["tgraph_x_nicks", "tgraph_y_nicks"],
				'tgraph_result_nicks', 'tgraph')

	def run(self, plotData=None):
		super(TGraphFromHistograms, self).run(plotData)

		for x, y, new in zip(plotData.plotdict["tgraph_x_nicks"], plotData.plotdict["tgraph_y_nicks"], plotData.plotdict["tgraph_result_nicks"]):
			log.debug("x-values from bin-contents of %s, y-values from bin-contents of %s (new nick: %s)" % (x, y, new))
			plotData.plotdict["root_objects"][new] = new_graph = ROOT.TGraphErrors()
			plotData.plotdict["nicks"].append(new)

			histo_iter = HistogramGroupIterator(
					histograms=(plotData.plotdict['root_objects'][x],plotData.plotdict['root_objects'][y]),
					skip=plotData.plotdict['tgraph_strip_empty']
			)
			for i, bin_contents in enumerate(histo_iter):
				(x_mean, x_err), (y_mean, y_err) = bin_contents
				new_graph.SetPoint(i, x_mean, y_mean)
				new_graph.SetPointError(i, x_err, y_err)
