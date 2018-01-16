#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import collections
import datetime
import os
import shlex
import string
import sys
import tempfile
import traceback

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError
for root_type in [
		ROOT.TFile, ROOT.TDirectory, ROOT.TDirectoryFile,
		ROOT.TTree, ROOT.TChain, ROOT.TNtuple,
		ROOT.TH1, ROOT.TH1F, ROOT.TH1D,
		ROOT.TH2, ROOT.TH2F, ROOT.TH2D,
		ROOT.TH3, ROOT.TH3F, ROOT.TH3D,
		ROOT.TProfile, ROOT.TProfile2D,
		ROOT.TGraph, ROOT.TGraphErrors, ROOT.TGraphAsymmErrors,
		ROOT.TGraph2D, ROOT.TGraph2DErrors,
		ROOT.TF1, ROOT.TF2, ROOT.TF3,
		ROOT.TCanvas, ROOT.TPad, ROOT.TLegend,
]:
	root_type.__init__._creates = True # https://root.cern.ch/phpBB3/viewtopic.php?t=9786

import Artus.Utility.jsonTools as jsonTools
import Artus.Utility.tools as tools
import Artus.HarryPlotter.core as harrycore


def pool_plot(args):
	try:
		return (args[0].plot(*args[1:]), None, None)
	except SystemExit as e:
		return (None, args[0].harry_args[args[1]], None)
	except Exception as e:
		return (None, args[0].harry_args[args[1]], traceback.format_exc())


class HarryPlotter(object):
	def __init__(self, list_of_config_dicts=None, list_of_args_strings=None, n_processes=1, n_plots=None, batch=None, standalone_executable=None):
		if standalone_executable is None:
			standalone_executable = os.path.expandvars("$CMSSW_BASE/src/Artus/HarryPlotter/scripts/standalone_harry.sh")
		self.standalone_executable = standalone_executable
		
		self.output_filenames = self.multi_plots(
				list_of_config_dicts=list_of_config_dicts,
				list_of_args_strings=list_of_args_strings,
				n_processes=n_processes,
				n_fast_plots=n_plots,
				batch=batch
		)
	
	def plot(self, plot_index):
		tmp_harry_args = self.harry_args[plot_index]
		harry_core = harrycore.HarryCore(args_from_script=tmp_harry_args)
		if not tmp_harry_args is None:
			log.debug("harry.py " + tmp_harry_args)
		output_filenames = harry_core.run()
		self.harry_cores[plot_index] = harry_core # TODO: thread-safe?
		return output_filenames
	
	def multi_plots(self, list_of_config_dicts, list_of_args_strings, n_processes=1, n_fast_plots=None, batch=None):
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
		
		if n_processes>1:
			for i in range(len(args_strings)):
				args_strings[i] += (" --hide-progressbar ")

		self.harry_args = []
		for config_dict, args_string in zip(config_dicts, args_strings):
			if config_dict is None:
				self.harry_args.append(None)
			else:
				config_dict["comment"] = " ".join(sys.argv)
				if not batch is None:
					config_dict["dry_run"] = True
				if "json_defaults" in config_dict:
					json_defaults_dict = jsonTools.JsonDict(config_dict["json_defaults"]).doIncludes().doComments()
					config_dict.pop("json_defaults")
					json_defaults_dict.update(config_dict)
					config_dict = json_defaults_dict
				self.harry_args.append("--json-defaults \"%s\"" % jsonTools.JsonDict(config_dict).toString(indent=None).replace("\"", "'"))
			
			if not args_string is None:
				if self.harry_args[-1] is None:
					self.harry_args[-1] = args_string
				else:
					self.harry_args[-1] += (" "+args_string)
				if config_dict is None:
					self.harry_args[-1] += (" --comment " + (" ".join(sys.argv)))
					if not batch is None:
						self.harry_args[-1] += " --dry-run"
		
		if not n_fast_plots is None:
			self.harry_args = self.harry_args[:n_fast_plots]
			n_plots = len(self.harry_args)
		
		self.harry_cores = [None]*n_plots
		
		# multi processing of multiple plots
		output_filenames = []
		failed_plots = []
		if (n_plots > 1) and (n_processes > 1):
			log.info("Creating {:d} plots in {:d} processes".format(n_plots, min(n_processes, n_plots)))
			results = tools.parallelize(pool_plot, zip([self]*n_plots, range(n_plots)), n_processes, description="Plotting")
			tmp_output_filenames, tmp_failed_plots, tmp_error_messages = zip(*([result for result in results if not result is None and result != (None,)]))
			output_filenames = [output_filename for output_filename in tmp_output_filenames if not output_filename is None]
			failed_plots = [(failed_plot, error_message) for failed_plot, error_message in zip(tmp_failed_plots, tmp_error_messages) if not failed_plot is None]
		
		# single processing of multiple plots
		elif n_plots > 1:
			log.info("Creating {:d} plots".format(n_plots))
			for plot_index in xrange(n_plots):
				try:
					output_filenames.append(self.plot(plot_index))
				except SystemExit as e:
					failed_plots.append((self.harry_args[plot_index], None))
				except Exception as e:
					log.info(str(e))
					failed_plots.append((self.harry_args[plot_index], None))
		
		# single plot
		elif n_plots > 0:
			output_filenames.append(self.plot(0))
		
		# batch submission
		if (not (batch is None)) and (len(failed_plots) < n_plots):
			workdir = tempfile.mkdtemp(prefix="harry_work_"+datetime.datetime.now().strftime("%Y-%m-%d_%H-%M")+"_")
			
			main_config = ""
			with open(os.path.expandvars("$CMSSW_BASE/src/Artus/HarryPlotter/data/grid-control_base_config.conf"), "r") as main_config_file:
				main_config = main_config_file.read()
			
			backend_config = ""
			with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Configuration/data/grid-control_backend_" + batch + ".conf"), "r") as backend_config_file:
				backend_config = backend_config_file.read()
			
			final_config = string.Template(main_config).safe_substitute(
					cmsswbase=os.path.expandvars("$CMSSW_BASE"),
					cwd=os.getcwd(),
					jsonconfigs="\n\t"+("\n\t".join([item[0] for item in output_filenames])),
					executable=self.standalone_executable,
					workdir=workdir,
					backend=backend_config
			)
			final_config_filename = workdir+".conf"
			with open(final_config_filename, "w") as final_config_file:
				final_config_file.write(final_config)
			
			command = "go.py " + final_config_filename
			log.info(command)
			logger.subprocessCall(shlex.split(command))
		
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

