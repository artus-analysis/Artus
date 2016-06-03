# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

"""
	copied from https://github.com/ksamdev/exo_plots/blob/master/root/tfile.py

	Created by Samvel Khalatyan, Feb 16, 2012
	Copyright 2011, All rights reserved
"""	

import ROOT
import sys


class TFileContextManager(object):
	"""
	Context manager for ROOT TFile
	
	Opening/closing file is automatically done, e.g.:
		with topen("input.root") as input_:
			plot = input_.Get("hello")
	the above code is equivalent to:
		input_ = ROOT.TFile("input.root")
		if not input_.IsZombie():
			plot = input_.Get("hello")
			input_.Close()
	"""
	def __init__(self, filename, mode = "readonly"):
		self._filename = filename
		self._file = None
		self._mode = mode

	def __enter__(self):
		"""Return actual ROOT file"""
		if self._file is None:
			self._file = ROOT.TFile.Open(self._filename, self._mode)
			# ROOT may have silently failed opening the file
			if (not self._file) or self._file.IsZombie():
				self._file = None
				# TFile::TFile "In case the file does not exist or is not a valid ROOT file, it is made a Zombie."
				raise IOError("No such ROOT file: %r" % self._filename)
		return self._file

	def __exit__(self, error_type, error_value, error_traceback):
		"""Close file on exit"""
		if self._file is not None:
			self._file.Close()
			self._file = None
