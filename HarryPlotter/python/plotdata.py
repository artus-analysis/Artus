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
import subprocess

import Artus.Utility.tools as tools


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
		self.metadata = {} # key: nick to which the value belongs
		self.input_json_dicts = []
		self.fit_results = {}

	@staticmethod
	def show_plot(filename, viewer):
		log.info("Opening Plot " + filename + " with " + viewer)
		FNULL = open(os.devnull, 'w')
		subprocess.Popen([viewer, filename], stdout=FNULL, stderr=FNULL)

	@staticmethod
	def show_plot_userpc(filename, viewer, user, userpc):
		"""Open the plot, but on the users desktop machine."""
		subprocess.call(['rsync', filename,
							  '%s:/usr/users/%s/plot.pdf' % (userpc, user)])

		# check if the imageviewer is running on the users local machine:
		p = subprocess.Popen(['ssh', userpc, 'ps', 'aux', '|', 'grep',
						'"%s /usr/users/%s/plot.%s"' % (viewer,
						user, filename.split(".")[-1]), '|', 'grep',
						 '-v', 'grep', '|', 'wc', '-l'], stdout=subprocess.PIPE)
		out, err = p.communicate()

		# if its not already running, start!
		if out[:1] == '0':
			print "\nOpening %s..." % viewer
			subprocess.Popen(['ssh', userpc,
			'DISPLAY=:0 %s /usr/users/%s/plot.%s &' % (viewer, user, filename.split(".")[-1])])

	@staticmethod
	def webplotting(www, output_dir, output_filenames=False,
			www_text=False, www_title="plots_archive",
			www_nodate=False, additional_output_files=False,
			save_legend=False, export_json=False, no_publish=False,
			www_no_overwrite=False, remote_subdir=None):
		# set some needed variables
		# user = tools.get_environment_variable("HARRY_REMOTE_USER")
		html_content = ""
		overview_filename = "index.html"
		date = datetime.date.today().strftime('%Y_%m_%d')
		if remote_subdir is None:
			remote_subdir = os.path.expandvars(os.path.join((date if (www == "" or not www_nodate) else ""), (www if type(www)==str else "")))
		url = os.path.expandvars(os.path.join("$HARRY_URL", remote_subdir, overview_filename))
		plots_for_gallery = [p for p in sorted(os.listdir(output_dir)) if (os.path.isfile(os.path.join(output_dir, p)) and all([not p.endswith("." + ext) for ext in ["json", "html"]]))]
		# get the html templates
		files_to_copy = []
		for galleryplot in plots_for_gallery:
			files_to_copy.append(os.path.join(output_dir, galleryplot))
		html_texts = {}
		for var in ['overview', 'description', 'plot']:
			with open(os.path.expandvars("$ARTUSPATH/HarryPlotter/data/template_webplotting_{}.html".format(var))) as htmlfile:
				html_texts[var] = string.Template(htmlfile.read())
		html_texts['description'] = html_texts['description'].substitute(url=url)
		if www_text:
			html_texts['description'] = www_text

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
				json=plot + ".json"
			)

		# put the html parts together and write
		with open(os.path.join(output_dir, overview_filename), "w") as overview_file:
			overview_file.write(html_texts['overview'].substitute(
				html_content=html_content,
				title=www_title,
				text=html_texts['description']
			))
		if no_publish:
			return 0

		# find out which files to copy
		if output_filenames is not False:
			files_to_copy = (output_filenames)
		files_to_copy += [os.path.join(output_dir, overview_filename)]

		if additional_output_files is not False:
			files_to_copy += additional_output_files
		if export_json is not False:
			files_to_copy += [export_json]
		if save_legend:
			files_to_copy += [os.path.join(output_dir, save_legend + _format) for _format in formats]

		# prepare the executed copy command
		web_plotting_copy_command = os.path.expandvars("$WEB_PLOTTING_COPY_COMMAND")

		# no overwriting

		if www_no_overwrite:
			web_plotting_ls_command = os.path.expandvars("$WEB_PLOTTING_LS_COMMAND").format(subdir=remote_subdir)
			process = subprocess.Popen(web_plotting_ls_command.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			remote_files, error = process.communicate()
			if error is not None or '[ERROR]' in remote_files:
				log.error('Error occured during web ls! Exiting. Output: %s' % (remote_files))
				raise
			remote_dir = web_plotting_ls_command.split()[-1]
			remote_files = [i[len(remote_dir):].strip('/') if i.startswith(remote_dir) else i for i in remote_files.split()]
			for i in files_to_copy:
				if 'index.html' not in i and i in remote_files:
					log.error('File %s already existing at %s. Exiting' % (i, remote_dir))
					raise

		# create remote dir, copy files
		mkdir_command = os.path.expandvars("$WEB_PLOTTING_MKDIR_COMMAND").format(subdir=remote_subdir)
		copy_command = web_plotting_copy_command.format(source=" ".join(files_to_copy), subdir=remote_subdir)

		log.debug("Copying plots to webspace...")
		log.debug("\nIssueing mkdir command: " + mkdir_command)
		logger.subprocessCall(mkdir_command.split())
		log.debug("\nIssueing copy command: " + copy_command)
		logger.subprocessCall(copy_command.split())
		log.debug("Copied {0}".format(" ".join([f.split("/")[-1] for f in files_to_copy])))
		log.info("Copied to: {0}".format(copy_command.split()[-1]))
		log.info("Website: {0}".format(url))


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
						PlotData.show_plot_userpc(output_filename, self.plotdict["live"], tools.get_environment_variable('USER'), userpc)
					else:
						PlotData.show_plot(output_filename, self.plotdict["live"])

			# web plotting
			# TODO: make this more configurable if users want to user other webspaces etc.
			if self.plotdict["www"] is not None:
				try:
					remote_subdir = self.plotdict["output_dir"].split('websync/')[1]
				except:
					logger.warning("Remote subdir may not reflect local gallery structure")
					remote_subdir = None

				self.webplotting(
					www=self.plotdict["www"],
					output_dir=self.plotdict["output_dir"],
					output_filenames=self.plotdict["output_filenames"],
					www_text=self.plotdict["www_text"],
					www_title=self.plotdict["www_title"],
					www_nodate=self.plotdict["www_nodate"],
					additional_output_files=self.plotdict["additional_output_files"] if "additional_output_files" in self.plotdict else False,
					save_legend=self.plotdict.get("save_legend", False),
					export_json=self.plotdict["export_json"],
					www_no_overwrite=self.plotdict["www_no_overwrite"] or self.plotdict["no_overwrite"],
					remote_subdir=remote_subdir,
				)

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

