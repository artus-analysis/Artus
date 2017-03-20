# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

"""
Based on https://github.com/ksamdev/exo_plots/blob/master/root/tfile.py

Created by Samvel Khalatyan, Feb 16, 2012
Copyright 2011, All rights reserved
"""	

import ROOT
import sys


class TFileContextManager(object):
	"""
	Minimal file-object interface for ROOT TFile
	
	This class replicates part of python's file management interface
	(``open``) for ROOT files. Most importantly, it can be used as a
	context manager:
	
	>>> from tfilecontextmanager import TFileContextManager as topen
	>>> with topen("data.root") as tfile:
	...     plot = tfile.Get("Hello")
	
	This will safely open and close the file, regardless of errors.
	"""
	def __init__(self, filename, mode = "readonly"):
		self._filename = filename
		self._mode = mode
		self._file = ROOT.TFile.Open(self._filename, self._mode)
		# ROOT may have silently failed opening the file
		# NOTE: didn't find what pyroot *actually* may return, this can probably be cut down - MF@030516
		if self._file is None or (not self._file) or self._file.IsZombie():
			self._file = None
			# TFile::TFile "In case the file does not exist or is not a valid ROOT file, it is made a Zombie."
			raise IOError("No valid ROOT file: %r" % self._filename)
		# Plain ``TFile(...)`` makes the file managed by Python, as expected
		# Using ``TFile.Open(...)`` makes the file owned by ROOT, because of reasons
		# See http://comments.gmane.org/gmane.comp.lang.c%2B%2B.root/15431

	def __enter__(self):
		"""Provide ROOT file safely"""
		# Return ROOT file, not self
		# self's management functions aren't needed in a context
		return self._file

	def __exit__(self, error_type, error_value, error_traceback):
		"""Close file on leaving context"""
		self.close()

	def close(self):
		"""Close ROOT file"""
		if self._file is not None:
			self._file.Close()
			self._file = None

	def __del__(self):
		# This class doesn't hold circular references, so __del__ is safe
		# See also:
		# - https://docs.python.org/3/reference/datamodel.html#object.__del__
		# - https://docs.python.org/3/library/io.html#io.IOBase.__del__
		if hasattr(self, '_file'):
			self.close()
