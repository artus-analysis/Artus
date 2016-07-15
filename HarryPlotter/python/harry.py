#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import collections

import ROOT
import sys
import traceback
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.jsonTools as jsonTools
import Artus.Utility.tools as tools
import Artus.HarryPlotter.core as harrycore


def pool_plot(args):
	try:
		return (args[0].plot(*args[1:]), None, None)
	except SystemExit as e:
		return (None, args[1:], None)
	except Exception as e:
		return (None, args[1:], traceback.format_exc())


class HarryPlotter(object):
	def __init__(self, list_of_config_dicts=None, list_of_args_strings=None, n_processes=1, n_plots=None):
		# load Kappa library
		try:
			ROOT.gSystem.Load("libKappa")
		except RuntimeError:
			log.debug("Could not load libKappa")

		self.output_filenames = self.multi_plots(
				list_of_config_dicts=list_of_config_dicts,
				list_of_args_strings=list_of_args_strings,
				n_processes=n_processes,
				n_fast_plots=n_plots
		)
	
	def plot(self, harry_args):
		harry_core = harrycore.HarryCore(args_from_script=harry_args)
		if not harry_args is None:
			log.debug("harry.py " + harry_args)
		return harry_core.run()
	
	def multi_plots(self, list_of_config_dicts, list_of_args_strings, n_processes=1, n_fast_plots=None):
		config_dicts = list_of_config_dicts if isinstance(list_of_config_dicts, collections.Iterable) and not isinstance(list_of_config_dicts, basestring) else [list_of_config_dicts]
		args_strings = list_of_args_strings if isinstance(list_of_args_strings, collections.Iterable) and not isinstance(list_of_args_strings, basestring) else [list_of_args_strings]
		
		# fill up missing list elements by repeating previous ones
		n_plots = max(len(config_dicts), len(args_strings))
		if (len(config_dicts) > 1) and (len(config_dicts) < n_plots):
			log.warning("Too few config dicts specified!")
		if (len(args_strings) > 1) and (len(args_strings) < n_plots):
			log.warning("Too few argument lists specified!")
		config_dicts = (config_dicts*n_plots)[:n_plots]
		args_strings = (args_strings*n_plots)[:n_plots]
		
		harry_args = []
		for config_dict, args_string in zip(config_dicts, args_strings):
			if config_dict is None:
				harry_args.append(None)
			else:
				config_dict["comment"] = " ".join(sys.argv)
				if "json_defaults" in config_dict:
					json_defaults_dict = jsonTools.JsonDict(config_dict["json_defaults"]).doIncludes().doComments()
					config_dict.pop("json_defaults")
					json_defaults_dict.update(config_dict)
					config_dict = json_defaults_dict
				harry_args.append("--json-defaults \"%s\"" % jsonTools.JsonDict(config_dict).toString(indent=None).replace("\"", "'"))
			
			if not args_string is None:
				if harry_args[-1] is None:
					harry_args[-1] = args_string
				else:
					harry_args[-1] += (" "+args_string)
				if config_dict is None:
					harry_args[-1] += (" --comment " + (" ".join(sys.argv)))
		if not n_fast_plots is None:
			harry_args = harry_args[:n_fast_plots]
		
		# multi processing of multiple plots
		output_filenames = []
		failed_plots = []
		if len(harry_args) > 1 and n_processes > 1:
			log.info("Creating {:d} plots in {:d} processes".format(len(harry_args), min(n_processes, len(harry_args))))
			results = tools.parallelize(pool_plot, zip([self]*len(harry_args), harry_args), n_processes)
			tmp_output_filenames, tmp_failed_plots, tmp_error_messages = zip(*([result for result in results if not result is None and result != (None,)]))
			output_filenames = [output_filename for output_filename in tmp_output_filenames if not output_filename is None]
			failed_plots = [(failed_plot, error_message) for failed_plot, error_message in zip(tmp_failed_plots, tmp_error_messages) if not failed_plot is None]
		
		# single processing of multiple plots
		elif len(harry_args) > 1:
			log.info("Creating {:d} plots".format(len(harry_args)))
			for harry_args in harry_args:
				try:
					output_filenames.append(self.plot(harry_args))
				except SystemExit as e:
					failed_plots.append((harry_args, None))
				except Exception as e:
					log.info(str(e))
					failed_plots.append((harry_args, None))
		
		# single plot
		elif len(harry_args) > 0:
			output_filenames.append(self.plot(harry_args[0]))
		
		if len(failed_plots) > 0:
			log.error("%d failed plots:" % len(failed_plots))
			for failed_plot in failed_plots:
				log.info("\n"+tools.get_colored_string("Failed plot:", color='red'))
				log.info("\t%s" % failed_plot[0])
				if failed_plot[1] is not None:
					try:
						log.info(tools.get_indented_text("    ", tools.get_colored_string("Traceback for this plot:", color='red')+"\n" + failed_plot[1]))
					except:
						log.info("\t Traceback for this plot: \n" + failed_plot[1])
		
		return output_filenames

