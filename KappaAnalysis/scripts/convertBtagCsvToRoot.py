#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import copy
import csv
import os

import Artus.HarryPlotter.harry as harry


def list_get(tmp_list, index, default):
	if (index >= 0) and (index < len(tmp_list)):
		return tmp_list[index]
	else:
		return default


def main():
	
	parser = argparse.ArgumentParser(description="Convert b-tag scale factor CSV file into ROOT file.", parents=[logger.loggingParser])
	parser.add_argument("-c", "--csv", help="CSV input file")
	parser.add_argument("-r", "--root", help="ROOT output file")
	args = parser.parse_args()
	logger.initLogger(args)
	
	indices = {}
	
	with open(args.csv) as csv_file:
		csv_reader = csv.reader(csv_file, delimiter=",")
		
		plot_config = {}
		plot_config["input_modules"] = ["InputInteractive"]
		plot_config["plot_modules"] = ["ExportRoot"]
		plot_config["filename"] = args.root.replace(".root", "")
				
		for row in csv_reader:
			row = [col.strip().replace("CSVv2;", "") for col in row]
			if "OperatingPoint" in row[0]:
				for index, col in enumerate(row):
					indices[col] = index
			else:
				
				formula = list_get(row, indices.get("formula", -1), "1.0").replace("\"", "")
				if not "x" in formula:
					formula = "({0}*(x**0.0))".format(formula)
				plot_config.setdefault("x_expressions", []).append(formula)
				
				if list_get(row, indices.get("OperatingPoint", -1), "0") == "3":
					plot_config.setdefault("x_bins", []).append(
							"100,"+list_get(row, indices.get("discrMin", -1), "0.0")+","+list_get(row, indices.get("discrMax", -1), "1.0")
					)
				else:
					plot_config.setdefault("x_bins", []).append(
							"100,"+list_get(row, indices.get("ptMin", -1), "0.0")+","+list_get(row, indices.get("ptMax", -1), "1.0")
					)
				
				plot_config.setdefault("labels", []).append(os.path.join(
						list_get(row, indices.get("OperatingPoint", -1), "0"),
						list_get(row, indices.get("measurementType", -1), "incl"),
						list_get(row, indices.get("sysType", -1), "central"),
						list_get(row, indices.get("jetFlavor", -1), "0"),
						"function"
				))
		
	harry_plotter = harry.HarryPlotter(list_of_config_dicts=[plot_config])


if __name__ == "__main__":
	main()

