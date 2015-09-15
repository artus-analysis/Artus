
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


"""
	This module contains a dictionary for binnings.
"""
class BinningsDict(object):
	def __init__(self, additional_binnings=None):
		self.binnings_dict = {}
		
		# example:
		# self.binnings_dict["zpt"] = "30 50 70 100 200 400 1000"
		
		if not additional_binnings is None:
			self.binnings_dict.update(additional_binnings)

	def get_binning(self, binning):
		return self.binnings_dict.get(binning, binning)
