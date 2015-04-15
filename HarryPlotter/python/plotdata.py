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
				remote_dir = tools.get_environment_variable('HARRY_REMOTE_DIR')+'/%s/%s/' % (date, (self.plotdict["www"] if type(self.plotdict["www"])==str else ""))
				remote_path = tools.get_environment_variable('HARRY_REMOTE_PATH') + '/%s' % remote_dir
				url = tools.get_environment_variable('HARRY_URL') + "/%s/%s" % (remote_dir, overview_filename)
				plots = sorted(os.listdir(self.plotdict["output_dir"]))
				content = ""
				n_plots_copied = 0

				log.info("Copying plots to webspace...")
				# loop over plots, make gallery
				for plot in [p for p in plots if (('.png' in p) or ('.pdf' in p))]:
					log.debug("File %s will be copied" % plot)
					# try to link to pdf file, if it exists
					href = plot.replace('.png', '.pdf')
					if href not in plots:
						href = plot
					title = plot.split('/')[-1][:-4].replace('_', ' ')
					content += htmlTemplatePlot % (title, href, title, plot)
					n_plots_copied += 1
				with open(self.plotdict["output_dir"] + '/' + overview_filename, 'w') as f:
					f.write(htmlTemplate % (url, content))
				if os.path.basename(url) not in plots:
					plots.append(os.path.basename(url))

				# create remote dir, copy plots and overview file
				create_dir_command = ['ssh', tools.get_environment_variable('HARRY_SSHPC'), 'mkdir -p', remote_path]
				log.debug("\nIssueing mkdir command: " + " ".join(create_dir_command))
				subprocess.call(create_dir_command)
				rsync_command =['rsync', '-u'] + [os.path.join(self.plotdict["output_dir"], p) for p in plots] + ["%s:%s" % (tools.get_environment_variable('HARRY_SSHPC'), remote_path)]
				log.debug("\nIssueing rsync command: " + " ".join(rsync_command) + "\n")
				subprocess.call(rsync_command)
				log.info("Copied %d plot(s) to %s" % (n_plots_copied, url))

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
