# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib

import ROOT
ROOT.TH1.SetDefaultSumw2(True)

import Artus.HarryPlotter.inputbase as inputbase


class InputRoot(inputbase.InputBase):
	def __init__(self):
		inputbase.InputBase.__init__(self)
	
	def run(self, plotdict=None):
		inputbase.InputBase.run(self, plotdict)

