# -*- coding: utf-8 -*-

"""
	plot run ranges and labels as dashed vertical lines on the plot
"""

import Artus.HarryPlotter.plotbase as plotbase

class PlotMplRunRanges(plotbase.PlotBase):

	def modify_argument_parser(self, parser, args):
		self.runrange_options = parser.add_argument_group(
			title="{} options".format(self.name()),
			description="Options for {} module ".format(self.name())
		)
		self.runrange_options.add_argument("--run-range-year", type=int, default=2012,
			help="Data-taking period for run ranges. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(PlotMplRunRanges, self).prepare_args(parser, plotData)
		if plotData.plotdict['run_range_year'] == 2012:
			self.runs = [[' 2012A', 190456.], [' 2012B', 193834.], [' 2012C', 197770.], [' 2012D', 203773.]]
		elif plotData.plotdict['run_range_year'] == 2015:
			self.runs = [[' 2015A', 248036.], [' 2015B', 251162.], [' 2015C', 253888.], [' 2015D', 256629.]]

	def run(self, plotData):
		x_min, x_max = plotData.plot.axes[0].dataLim.min[0], plotData.plot.axes[0].dataLim.max[0]
		for [runlabel, runnumber] in self.runs:
			# TODO make drawing options configurable?
			if x_min <= runnumber <= x_max:
				x_pos = (runnumber - x_min) / (x_max - x_min)
				if 0 < x_pos < 0.95:
					plotData.plot.axes[0].text(
						x_pos,
						0.92, runlabel, transform=plotData.plot.axes[0].transAxes, va='top',
						ha='left', color='gray', alpha=0.7, size='medium'
					)
				for ax in plotData.plot.axes:
					ax.axvline(runnumber, color='gray', linestyle='--', alpha=0.2)
