#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.core as harrycore


def harry(args_from_script = None):
	"""
	Main plotting function
	
	Can be called from within python scripts by passing the arguments as a string
	as it would be done by calling this script in the bash.
	"""

	harry_core = harrycore.HarryCore()
	harry_core.run(args_from_script)

