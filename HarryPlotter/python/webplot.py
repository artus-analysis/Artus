
# -*- coding: utf-8 -*-

import os
import string


def get_inputs(input_dir, **kwargs):
	file_types = kwargs.get("file_types", ["png", "jpg", "pdf", "svg", "eps"])
	
	input_dir_content =	os.listdir(input_dir)
	
	plot_files = {}
	subdirs = []
	for input_object in input_dir_content:
		full_input_object = os.path.join(input_dir, input_object)
		
		if os.path.isfile(full_input_object):
			split_input_object = os.path.splitext(input_object)
			extension = split_input_object[-1].replace(".", "").lower()
			
			if extension in file_types:
				plot_files.setdefault(split_input_object[0], []).append(extension)
		
		else:
			subdirs.append(full_input_object)
	
	for filename, extensions in plot_files.iteritems():
		extensions.sort(key=lambda extension: file_types.index(extension))
	subdirs.sort()
	
	return plot_files, subdirs


def webplot(input_dir, **kwargs):

	plot_files, subdirs = get_inputs(input_dir, **kwargs)
	html_texts = {}
	for var in ["overview", "description", "subdir", "plot", "link"]:
		with open(os.path.expandvars("$ARTUSPATH/HarryPlotter/data/template_webplot_{}.html".format(var))) as htmlfile:
			html_texts[var] = string.Template(htmlfile.read())

	# create remote dir
	if not kwargs.get("copy", False):
		print "Creating directory",input_dir,"..."
		mkdir_command = os.path.expandvars("$WEB_PLOTTING_MKDIR_COMMAND").format(subdir=input_dir)
		os.system(mkdir_command)

	# treat subdirs recursively
	html_desciption = ""
	if kwargs.get("recursive", False):
		html_subdirs = ""
		for subdir in subdirs:
			webplot(subdir, **kwargs)
			html_subdirs += html_texts["subdir"].substitute(subdir=os.path.relpath(subdir, input_dir))
		html_desciption = html_texts["description"].substitute(subdirs=html_subdirs)
	else:
		html_desciption = html_texts["description"].substitute(subdirs="")
	
	html_plots = ""
	for filename, extensions in plot_files.iteritems():
		html_plots += html_texts["plot"].substitute(
				plot=filename+"."+extensions[0],
				title=filename,
				links=" ".join([html_texts["link"].substitute(
						plot=filename+"."+extension,
						title=filename,
						extension=extension
				) for extension in extensions])
		)
	
	html_index_filename = os.path.join(input_dir, "index.html")
	with open(html_index_filename, "w") as html_index_file:
		html_index_file.write(html_texts["overview"].substitute(
				title=kwargs.get("title", input_dir),
				description=html_desciption,
				plots=html_plots
		))
		#print "Created", html_index_filename

	# copy files
	if not kwargs.get("copy", False):
		full_input_dir = os.path.abspath(input_dir)
		input_dir_content = os.listdir(full_input_dir)
		for input_object in input_dir_content:
			full_input_object = os.path.join(full_input_dir, input_object)
			if os.path.isfile(full_input_object):
				print "Copying",input_object,"into",input_dir,"..."
				copy_command = os.path.expandvars("$WEB_PLOTTING_COPY_COMMAND").format(source=full_input_object, subdir=input_dir)
				os.system(copy_command)
