# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc


class PlotData(object):
	"""
	Up to now, this is just a wrapper of the plotdict.
	
	One could think about structuring the memebers a bit more. Ideas are
	- one container for one plots (including files, histograms, plotting settings, ...)
	- Formatter classes
	"""

	def __init__(self, args):
		super(PlotData, self).__init__()
		self.plotdict = args
		self.plot = None
	
	def __del__(self):
		for root_object in self.plotdict.get("root_objects", []):
			del(root_object)
	
	def save(self):
		self.plot.finish()
		if not self.plot is None:
			for filename in self.plotdict["output_filenames"]:
				self.plot.save(filename)
				log.info("Created plot \"%s\"." % filename)
			return self.plotdict["output_filenames"]


class PlotContainer(object):
	""" Abstract container for plots """
	
	def finish(self):
		""" Overwrite this function to define how a plot is finished. """
		pass
	
	@abc.abstractmethod
	def save(self, filename):
		""" Overwrite this function to define how a plot is saved. """
		pass

