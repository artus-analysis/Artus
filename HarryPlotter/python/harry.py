#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import collections
from multiprocessing import Pool

import Artus.HarryPlotter.core as harrycore


def pool_plot(args):
	try:
		args[0].plot(*args[1:])
		return None
	except SystemExit as e:
		return args[1:]
	except Exception as e:
		log.info(str(e))
		return args[1:]

class HarryPlotter(object):
	def __init__(self, plots=None, n_processes=1):
		self.multi_plots(plots if isinstance(plots, collections.Iterable) and not isinstance(plots, basestring) else [plots],
		                 n_processes=n_processes)
	
	def plot(self, harry_args):
		harry_core = harrycore.HarryCore(args_from_script=harry_args)
		harry_core.run()
	
	def multi_plots(self, list_of_harry_args, n_processes=1):
		failed_plots = []
		
		# multi processing of multiple plots
		if len(list_of_harry_args) > 1 and n_processes > 1:
			pool = Pool(processes=n_processes)
			results = pool.map_async(pool_plot, zip([self]*len(list_of_harry_args), list_of_harry_args))
			failed_plots = [result for result in results.get() if not result is None and result != (None,)]
		
		# single processing of multiple plots
		elif len(list_of_harry_args) > 1:
			for harry_args in list_of_harry_args:
				try:
					self.plot(harry_args)
				except SystemExit as e:
					failed_plots.append(harry_args)
				except Exception as e:
					log.info(str(e))
					failed_plots.append(harry_args)
		
		# single plot
		elif len(list_of_harry_args) > 0:
			self.plot(list_of_harry_args[0])
		
		if len(failed_plots) > 0:
			log.error("%d failed plots:" % len(failed_plots))
			for failed_plot in failed_plots:
				log.info("\t%s" % failed_plot)

