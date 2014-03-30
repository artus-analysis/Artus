#!/usr/bin/env python
# -*- coding: utf-8 -*-

#### todo: re-enable live plotting
#### todo: allow push to webserver
#### todo: enable 2D plotting with stacked inputs
#### todo: allow formula plotting
#### todo: use dictionaries

import Artus.HarryPlotter.harrycore as harrycore


if __name__ == "__main__":
	"""This script executes all steps necessary to create a plot."""
	print "\nHarry Plotter - the plot wizard\n"

	basedict = harrycore.get_basic_dictionary()
	parser = harrycore.get_basic_parser(**basedict)
	plotdict = harrycore.create_dictionary_from_parser(parser)
	harrycore.plot(plotdict)
