#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import sys

import HarryPlotter.Plotting.harry as harry


if __name__ == "__main__" and len(sys.argv) > 1:
	"""This script executes all steps necessary to create a plot."""
	harry.harry()

