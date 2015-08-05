# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc
import datetime
import subprocess
import os

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
				overview_filename = 'overview.html'
				date = datetime.date.today().strftime('%Y_%m_%d')
				remote_dir = os.path.join(tools.get_environment_variable('HARRY_REMOTE_DIR'), date, (self.plotdict["www"] if type(self.plotdict["www"])==str else ""))
				remote_path = os.path.join(tools.get_environment_variable('HARRY_REMOTE_PATH'), remote_dir)
				url = os.path.join(tools.get_environment_variable('HARRY_URL'), remote_dir, overview_filename)
				plots_for_gallery = [p for p in sorted(os.listdir(self.plotdict["output_dir"])) if (('.png' in p) or ('.pdf' in p))]
				plots_to_copy = [os.path.basename(filename), overview_filename]
				if self.plotdict.get('save_legend', False):
					for f in self.plotdict['formats']:
						plots_to_copy.append(".".join([self.plotdict['save_legend'], f]))

				html_content = ""

				log.info("Copying plots to webspace...")
				# loop over plots, make gallery
				for plot in [p for p in plots_for_gallery if (('.png' in p) or ('.pdf' in p))]:
					# try to link to pdf file, if it exists
					href = plot.replace('.png', '.pdf')
					if href not in plots_for_gallery:
						href = plot
					title = plot.split('/')[-1][:-4].replace('_', ' ')
					html_content += htmlTemplatePlot % (title, href, title, plot)
				with open(os.path.join(self.plotdict["output_dir"], overview_filename), 'w') as f:
					f.write(htmlTemplate % (url, html_content))

				# create remote dir, copy plots and overview file
				create_dir_command = ['ssh', user+'@'+tools.get_environment_variable('HARRY_SSHPC'), 'mkdir -p', remote_path]
				log.debug("\nIssueing mkdir command: " + " ".join(create_dir_command))
				subprocess.call(create_dir_command)
				rsync_command =['rsync', '-u'] + [os.path.join(self.plotdict["output_dir"], p) for p in plots_to_copy] + ["%s@%s:%s" % (user, tools.get_environment_variable('HARRY_SSHPC'), remote_path)]
				log.debug("\nIssueing rsync command: " + " ".join(rsync_command) + "\n")
				subprocess.call(rsync_command)
				log.info("Copied {0}; see {1}".format(filename.split("/")[-1], url))

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


# these html templates are needed to create the web galleries
htmlTemplate = """<!DOCTYPE html>
<html>
<head>
<style type="text/css">
div { float:left; }
pre { display: inline; padding: 3px 7px; font-size: 16px; background-color: #F5F5F5; border: 1px solid rgba(0, 0, 0, 0.15); border-radius: 4px; }
h3 { color: #888; font-size: 16px; }
</style>
</head>
<body>
<h1>Plot overview</h1>
<p>A <a href=".">file list</a> is also available and all plots can be downloaded using</p>
<p><code>wget -r -l 1 %s</code></p>
%s
</body>
</html>
"""
htmlTemplatePlot = """<div><h3>%s</h3><a href="%s" title="%s"><img src="%s" height="400"></a></div>\n"""
