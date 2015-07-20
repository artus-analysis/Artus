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
		"""Context management entry point"""

		if self._file is None:
			self._file = ROOT.TFile.Open(self._filename, self._mode)
			if (not self._file) or self._file.IsZombie():
				self._file = None
				log.critical("Failed to open ROOT file {0!r}!".format(self._filename))
				sys.exit(1)
				#raise RuntimeError("failed to open file {0!r}".format(self._filename))

		return self._file

	def __exit__(self, error_type, error_value, error_traceback):
		"""Exit the context management"""

		if not error_type and self._file:
			self._file.Close()
			self._file = None
