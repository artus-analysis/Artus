# -*- coding: utf-8 -*-

"""
	Plot a rectangle. Can e.g. be used to shade certain parts of a plot.

	Basic usage: harry.py HARRY-ARGS --plot-modules PlotMpl PlotMplRectangle  --rectangle-x 0 30 [RECTANGLE-ARGS]

	Further documentation on the used MPL object: http://matplotlib.org/api/patches_api.html#matplotlib.patches.Rectangle
"""

import matplotlib.patches as patches
import Artus.HarryPlotter.plotbase as plotbase


class PlotMplRectangle(plotbase.PlotBase):

	def modify_argument_parser(self, parser, args):
		self.rectangle_options = parser.add_argument_group(
			title="{} options".format(self.name()),
			description="Multiple rectangles may be drawn by supplying multiple inputs (e.g. 2 x-pairs, 1 y-pair and 2 alphas). Shorter parameters are expanded with default values."
		)
		# limits
		self.rectangle_options.add_argument("--rectangle-x", nargs="*", type=float, default=[],
			help="x coordinate pairs of the rectangle. [Default: entire x range]")
		self.rectangle_options.add_argument("--rectangle-y", nargs="*", type=float, default=[],
			help="y coordinate pairs of the rectangle. [Default: entire y range")
		self.rectangle_options.add_argument("--rectangle-relative", default=False, action="store_true",
			help="Take x,y limits in relative (0-1) axis coordinates, not in absolute data units.")
		# formatting: color etc.
		self.rectangle_options.add_argument("--rectangle-alpha", nargs="*", type=float, default=[0.5],
			help="alpha value of the rectangle (0=transparent, 1=opaque). [Default:%(default)s]")
		self.rectangle_options.add_argument("--rectangle-color", nargs="*", type=str, default=['grey'],
			help="color of the rectangle. [Default: %(default)s]")

	@staticmethod
	def _plot_rectangle_count(plotData):
		return max(
			len(plotData.plotdict['rectangle_x']) / 2,
			len(plotData.plotdict['rectangle_y']) / 2,
			len(plotData.plotdict['rectangle_alpha']),
			len(plotData.plotdict['rectangle_color']),
		)

	def prepare_args(self, parser, plotData):
		super(PlotMplRectangle, self).prepare_args(parser, plotData)
		assert plotData.plotdict['rectangle_x'] is None or len(plotData.plotdict['rectangle_x']) % 2 == 0, "x coordinates must be (multiple) pairs"
		assert plotData.plotdict['rectangle_y'] is None or len(plotData.plotdict['rectangle_y']) % 2 == 0, "y coordinates must be (multiple) pairs"
		# expand missing parameters with defaults
		rectangle_count = self._plot_rectangle_count(plotData)
		plotData.plotdict['rectangle_alpha'] += parser.get_default('rectangle_alpha')*(rectangle_count - len(plotData.plotdict['rectangle_alpha']))
		plotData.plotdict['rectangle_color'] += parser.get_default('rectangle_color')*(rectangle_count - len(plotData.plotdict['rectangle_color']))


	def run(self, plotData):
		for rect_id in xrange(self._plot_rectangle_count(plotData)):
			for ax in plotData.plot.axes:
				# use plot limits for missing x or y pairs
				x_coords = plotData.plotdict['rectangle_x'][rect_id*2:rect_id*2+2] or ax.get_xlim()
				y_coords = plotData.plotdict['rectangle_y'][rect_id*2:rect_id*2+2] or ax.get_ylim()
				ax.add_patch(
					patches.Rectangle(
						(x_coords[0], y_coords[0]),
						x_coords[1]-x_coords[0],  # width
						y_coords[1]-y_coords[0],  # height
						facecolor=plotData.plotdict['rectangle_color'][rect_id],
						alpha=plotData.plotdict['rectangle_alpha'][rect_id],
						edgecolor=None,
						linewidth=0,
						**({'transform': ax.transAxes} if plotData.plotdict['rectangle_relative'] else {})
					)
				)
