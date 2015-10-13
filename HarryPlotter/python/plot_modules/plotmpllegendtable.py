# -*- coding: utf-8 -*-

"""
	Plot the legend in tabular format in extra file.

	Basic usage: harry.py HARRY-ARGS --plot-modules PlotMpl PlotMplLegendTable --legend-table-column-headers a b --legend-table-row-headers 1 2 3
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import numpy as np
import os
from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt

import Artus.HarryPlotter.plotbase as plotbase
from Artus.HarryPlotter.plot_modules.plotmpl import PlotMpl


class PlotMplLegendTable(plotbase.PlotBase):

	def modify_argument_parser(self, parser, args):
		super(PlotMplLegendTable, self).modify_argument_parser(parser, args)
		self.legendtable_options = parser.add_argument_group(title="{} options".format(self.name()), description="legend table")
		self.legendtable_options.add_argument("--legend-table-column-headers", nargs="*", type=str,
		                                     help="Names of the column headers")
		self.legendtable_options.add_argument("--legend-table-row-headers", nargs="*", type=str,
		                                     help="Names of the row headers")
		self.legendtable_options.add_argument("--legend-table-invert", action="store_true",
		                                     help="Invert legend handle columns/rows")
		self.legendtable_options.add_argument("--legend-table-filename", type=str, default='legend_table',
		                                     help="Filename for the legend table. [Default: %(default)s]")

	def run(self, plotData):

		fig = plt.figure(figsize=(1,1))
		ax = fig.add_subplot(111, frameon=False)
		ax.xaxis.set_visible(False)
		ax.yaxis.set_visible(False)

		column_headers = plotData.plotdict['legend_table_column_headers']
		row_headers = plotData.plotdict['legend_table_row_headers']

		n_columns = len(column_headers)
		n_rows = len(row_headers)
		# blank rectangle as handle for index column/row
		rectangle = Rectangle((0, 0), 1, 1, fc="w", fill=False, edgecolor='none', linewidth=0)

		labels = plotData.plot.labels
		# switch rows and colum handles:
		if plotData.plotdict['legend_table_invert']:
			inverted_labels = []
			for i_column in range(n_columns):
				inverted_labels += [labels[i*(n_columns)+i_column] for i in range(n_rows)]
			labels = inverted_labels
		handles, labels = PlotMpl.get_legend_handles_labels_ordered(plotData.plot.axes[0], labels)

		# check
		if (n_rows * n_columns != len(handles)):
			log.warning("'N_columns ({}) times N_rows ({})' does not correspond to number of legend handles ({})!".format(n_columns, n_rows, len(handles)) )

		# get handles
		legend_handles = [rectangle]*(n_rows+1)
		for i_column in range(n_columns):
			legend_handles += [rectangle] + [handles[i] for i in range(n_rows*i_column,n_rows*(i_column+1) )]

		# get labels
		labels = [""] + row_headers
		for i_column in range(n_columns):
			labels += [column_headers[i_column]] + [""] * n_rows
		legend_labels = np.concatenate([labels])

		# make legend
		ax.legend(legend_handles, legend_labels,
				ncol = (n_columns+1), handletextpad = -2,
				loc='center', frameon=False)

		# save figure
		legend_filenames = []
		for filename in plotData.plotdict['output_filenames']:
			legend_filename = os.path.join(os.path.dirname(filename), plotData.plotdict['legend_table_filename']+os.path.splitext(filename)[-1])
			fig.savefig(
						legend_filename,
						# TODO cleanly crop figure to legend size
						# the arguments below are just workarounds :(
						bbox_inches='tight',
						pad_inches=0.1
			)
			legend_filenames += [legend_filename]
		if "additional_output_files" not in plotData.plotdict:
			plotData.plotdict["additional_output_files"] = []
		plotData.plotdict["additional_output_files"] += legend_filenames
