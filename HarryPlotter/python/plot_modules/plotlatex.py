#  -*- coding: utf-8 -*-

import os
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata

from Artus.HarryPlotter.utility.mplhisto import MplHisto
import Artus.HarryPlotter.utility.labels as labels

class LaTeXContainer(plotdata.PlotContainer):
	def __init__(self):
		pass
	def finish(self):
		self.whole_string = ""
		self.whole_string += self.latex_head + "\n"
		self.whole_string += self.latex_title + "\n"
		self.whole_string += self.latex_table_format + "\n"
		self.whole_string += self.main_table + "\n"
		self.whole_string += self.latex_footer + "\n"

	def save(self, filename):
		texfile_name = os.path.splitext(filename)[0] + ".tex"
		output_file = open(texfile_name, "w")
		output_file.write(self.whole_string)
		output_file.close()
		os.system("pdflatex " + " -output-directory=" + os.path.dirname(filename) + " " + texfile_name + " >> /dev/null" )

class PlotLaTeX(plotbase.PlotBase):
	"""Create tables for LaTeX from Histograms."""

	def __init__(self):
		super(PlotLaTeX, self).__init__()
	def modify_argument_parser(self, parser, args):
		super(PlotLaTeX, self).modify_argument_parser(parser, args)

	def prepare_args(self, parser, plotData):
		plotData.plotdict["formats"] = ["pdf"]
		super(PlotLaTeX, self).prepare_args(parser, plotData)

	def run(self, plotData):
		super(PlotLaTeX, self).run(plotData)

	def set_style(self, plotData):
		super(PlotLaTeX, self).set_style(plotData)

	def create_canvas(self, plotData):
		plotData.plot = LaTeXContainer()
		plotData.plot.latex_head = r"""
\documentclass[12pt]{article}
\usepackage[table]{xcolor}% http://ctan.org/pkg/xcolor
\begin{document}"""
		plotData.plot.latex_footer = r"""
\end{tabular}
\end{document}"""

	def make_plots(self, plotData):
		for nick in plotData.plotdict["nicks"]:
			root_object = plotData.plotdict["root_objects"][nick]
		if isinstance(root_object, ROOT.TH2):
			self.mplhist = MplHisto(root_object)

		self.table = self.mplhist.bincontents

		plotData.plot.main_table = self.tableToLaTeX(self.table, plotData.plotdict["y_tick_labels"], plotData.plotdict["x_tick_labels"])

	def modify_axes(self, plotData):
		super(PlotLaTeX, self).modify_axes(plotData)
		self.table = self.mplhist.bincontents

	def add_labels(self, plotData):
		super(PlotLaTeX, self).add_labels(plotData)
		if plotData.plotdict["title"] != None:
			plotData.plot.latex_title = r"\section*{" + plotData.plotdict["title"] + r"}"
		else:
			plotData.plot.latex_title = ""
		plotData.plot.latex_table_format = r"\begin{tabular}{ | "
		for x_tick in range(len(plotData.plotdict["x_tick_labels"])+1):
			plotData.plot.latex_table_format += "c | "
		plotData.plot.latex_table_format += "}"


	def add_texts(self, plotData):
		super(PlotLaTeX, self).add_texts(plotData)

	@staticmethod
	def tableToLaTeX(table, rowLabels, colLabels):
		delimiter_cells = "\t&\t"
		delimiter_rows = '\t \\\\ \\hline \n'
		result = "\hline \n" + delimiter_cells
		for colLabel in colLabels[:-1]:
			result += colLabel + delimiter_cells
		result += colLabels[-1] + delimiter_rows
		for rowLabel, row in zip(rowLabels, table):
			result += rowLabel + delimiter_cells
			for col in row[:-1]:
				result += str(col) + delimiter_cells
			result += str(row[-1])
			result += delimiter_rows
		return result



