# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc
import datetime
import glob
import os
import string

import Artus.Utility.tools as tools
import Artus.HarryPlotter.utility.extrafunctions as extrafunctions

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
		self.input_json_dicts = []
		self.fit_results = {}
	
	def __del__(self):
		for root_object in self.plotdict.get("root_objects", []):
			del(root_object)
	
	def save(self):
		self.plot.finish()
		if not self.plot is None:
			for filename in self.plotdict["output_filenames"]:
				self.plot.save(filename)
				log.info("Created plot \"%s\"." % filename)

			if not (self.plotdict["live"]==None):
				# if 'userpc', get the username and name of desktop machine
				if self.plotdict["userpc"]:
					userpc = ("%s@%s" % (tools.get_environment_variable("USER"), tools.get_environment_variable("HARRY_USERPC"))).replace("\n", "")

				for output_filename in self.plotdict["output_filenames"]:
					if self.plotdict["userpc"]:
						extrafunctions.show_plot_userpc(output_filename, self.plotdict["live"], tools.get_environment_variable('USER'), userpc)
					else:
						extrafunctions.show_plot(output_filename, self.plotdict["live"])

			# web plotting
			# TODO: make this more configurable if users want to user other webspaces etc.
			if self.plotdict["www"] != None:
				# set some needed variables
				user = tools.get_environment_variable("HARRY_REMOTE_USER")
				html_content = ""
				overview_filename = 'overview.html'
				date = datetime.date.today().strftime('%Y_%m_%d')
				remote_dir = os.path.expandvars(os.path.join("$HARRY_REMOTE_DIR", date, (self.plotdict["www"] if type(self.plotdict["www"])==str else "")))
				remote_path = os.path.expandvars(os.path.join("$HARRY_REMOTE_PATH", remote_dir))
				url = os.path.expandvars(os.path.join("$HARRY_URL", remote_dir, overview_filename))
				plots_for_gallery = [p for p in sorted(os.listdir(self.plotdict["output_dir"])) if (os.path.isfile(os.path.join(self.plotdict["output_dir"], p)) and all([not p.endswith("."+ext) for ext in ["json", "html", "root"]]))]

				# get the html templates
				html_texts = {}
				for var in ['overview', 'description', 'plot']:
					with open(os.path.expandvars("$ARTUSPATH/HarryPlotter/data/template_webplotting_{}.html".format(var))) as htmlfile:
						html_texts[var] = string.Template(htmlfile.read())
				html_texts['description'] = html_texts['description'].substitute(url=url)
				if self.plotdict["www_text"]:
					html_texts['description'] = self.plotdict["www_text"]

				# loop over plots, make gallery (one entry for multiple formats)
				for plot in sorted(list(set([os.path.splitext(plot)[0] for plot in plots_for_gallery]))):
					formats = [os.path.splitext(p)[1] for p in plots_for_gallery if (plot == os.path.splitext(p)[0])]
					# use png for preview, if it exists
					image = plot + ('.png' if (plot +'.png' in plots_for_gallery) else formats[0])

					# links for the different formats
					links = ""
					for fileformat in formats:
						links +=' <a href="{}">{}</a>'.format(plot+fileformat, fileformat[1:])

					html_content += html_texts['plot'].substitute(
							title=plot,
							image=image,
							links=links,
							json=plot+".json"
					)

				# put the html parts together and write
				with open(os.path.join(self.plotdict["output_dir"], overview_filename), "w") as overview_file:
					overview_file.write(html_texts['overview'].substitute(
						html_content=html_content,
						title=self.plotdict["www_title"],
						text=html_texts['description']
					))

				# find out which files to copy
				files_to_copy = (
					self.plotdict["output_filenames"]
					+ [os.path.join(self.plotdict["output_dir"], overview_filename)]
				)
				if "additional_output_files" in self.plotdict:
					files_to_copy += self.plotdict["additional_output_files"]
				if self.plotdict["export_json"]:
					files_to_copy += [self.plotdict["export_json"]]
				if self.plotdict.get("save_legend", False):
					files_to_copy += [os.path.join(self.plotdict["output_dir"], ".".join([self.plotdict["save_legend"], _format])) for _format in self.plotdict["formats"]]

				# create remote dir, copy files
				log.info("Copying plots to webspace...")
				sshpc = tools.get_environment_variable("HARRY_SSHPC")
				create_dir_command = ["ssh", user+"@"+sshpc, "mkdir -p", remote_path]
				log.debug("\nIssueing mkdir command: " + " ".join(create_dir_command))
				logger.subprocessCall(create_dir_command)
				rsync_command = ["rsync", "-u"] + files_to_copy + ["%s@%s:%s" % (user, sshpc, remote_path)]
				log.debug("\nIssueing rsync command: " + " ".join(rsync_command) + "\n")
				logger.subprocessCall(rsync_command)
				log.info("Copied {0}; see {1}".format(" ".join([f.split("/")[-1] for f in self.plotdict["output_filenames"]]), url))

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

