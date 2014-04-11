# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.inputbase as InputBase


class InputRoot(inputbase.InputBase):
	def __init__(self):
		inputbase.InputBase.__init__(self)


