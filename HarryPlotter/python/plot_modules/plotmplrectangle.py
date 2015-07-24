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
		self.rectangle_options = parser.add_argument_group("{} options".format(self.name()))
		# limits
		self.rectangle_options.add_argument("--rectangle-x", nargs="*", type=float, default=[0, 1],
			help="x coordinates of the rectangle. [Default: %(default)s]")
		self.rectangle_options.add_argument("--rectangle-y", nargs="*", default=None,
			help="y coordinates of the rectangle. [Default: entire y range")
		self.rectangle_options.add_argument("--rectangle-relative", default=False, action="store_true",
			help="Take x,y limits in relative (0-1) axis coordinates, not in absolute data units.")
		# formatting: color etc.
		self.rectangle_options.add_argument("--rectangle-alpha", type=float, default=0.5,
			help="alpha value of the rectangle (0=transparent, 1=opaque). [Default:%(default)s]")
		self.rectangle_options.add_argument("--rectangle-color", type=str, default='grey',
			help="color of the rectangle. [Default: %(default)s]")


	def run(self, plotData):
		for ax in plotData.plot.axes:
			if plotData.plotdict['rectangle_y'] is None:
				plotData.plotdict['rectangle_y'] = ax.get_ylim()
			ax.add_patch(
				patches.Rectangle(
					(plotData.plotdict['rectangle_x'][0], plotData.plotdict['rectangle_y'][0]),
					plotData.plotdict['rectangle_x'][1]-plotData.plotdict['rectangle_x'][0],  # width
					plotData.plotdict['rectangle_y'][1]-plotData.plotdict['rectangle_y'][0],  # height
					facecolor=plotData.plotdict['rectangle_color'],
					alpha=plotData.plotdict['rectangle_alpha'],
					edgecolor=None,
					linewidth=0,
					**({'transform': ax.transAxes} if plotData.plotdict['rectangle_relative'] else {})
				)
			)















