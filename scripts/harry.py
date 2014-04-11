#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.core as harrycore


if __name__ == "__main__":
	"""This script executes all steps necessary to create a plot."""

	harry_core = harrycore.HarryCore()
	harry_core.run()

