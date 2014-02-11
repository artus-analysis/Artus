#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""This is the main analysis program"""

import argparse
import glob
import imp
import json
import os
import errno
import shutil
import subprocess
import sys
import time


def artus():
	"""artus modifies and runs the configs"""
	aborted = False
	options = getoptions()
	if not options.nologo:
		print logo()

	if options.delete:
		try:
			subprocess.call(['go.py', options.work + "/" + options.out + ".conf", "-d all"])
		except:
			print "could not delete currently running jobs"
			exit(1)
		try:
			shutil.rmtree(options.work)
			print "Directory %s deleted." % options.work
		except:
			print "Could not delete output directory %s" % options.work
		exit(0)
		

	# make json config
	if not options.isjson and not options.resume:
		custom = imp.load_source("config", options.cfg)
		conf = custom.config()
		conf["InputFiles"] = createFileList(conf["InputFiles"], options.fast)
		if conf["OutputPath"] == "out":
			conf["OutputPath"] = options.out
		if options.skip:
			conf['EventCount'] = options.skip[1]
			conf['SkipEvents'] = options.skip[0]
		if options.printconfig:
			print "json config:"
			print json.dumps(conf, sort_keys=True, indent=4)
		if not options.resume:
			writeJson(conf, options.json)
			print len(conf["Pipelines"]),
			print "pipelines configured, written to", options.json

	# exit here if json config was the only aim
	if options.config:
		exit(0)

	# Now the config .json is ready and we can run Artus
	if not os.path.exists('artus'):
		print "Artus is not found, please compile it first."
		exit(1)
	if options.batch:  # this works in a specific setup, to be discussed
		if not options.resume:
			prepareWork(options.work, options.out, options.clean)
			writeDBS(conf, options.out, options.work + "/files.dbs")
			createRunfile(options.json, options.work + "/run-artus.sh", workpath = options.work)
			shutil.copy(options.json, options.work)
			shutil.copy("artus", options.work)
			outpath = createGridControlConfig(conf, options.work + "/" + options.out + ".conf", timestamp = options.timestamp)
			outpath = options.work + "out/" + outpath
		else:
			outpath = options.work + "out/*.root"

		try:
			subprocess.call(['go.py', options.work + "/" + options.out + ".conf"])
		except KeyboardInterrupt:
			print "go.py %s/%s.conf" % (options.work, options.out)
			exit(0)
		except:
			print "grid-control run failed"
			exit(1)

		if glob.glob(outpath):
			subprocess.call(['hadd', options.work + 'out.root'] + glob.glob(outpath))
		else:
			print "Batch job did not produce output %s. Exit." % outpath
			exit(1)

		try:
			print "Symlink to output file created: ", "%s/work/%s.root" % (getPath(), options.out)
			if not os.path.exists(getPath() + "work/"):
				os.makedirs(getPath() + "work/")
			os.symlink(options.work + "out.root", "%s/work/%s.root" % (getPath(), options.out))
		except OSError, e:
			if e.errno == errno.EEXIST:
				os.remove("%s/work/%s.root" % (getPath(), options.out))
				os.symlink(options.work + "out.root", "%s/work/%s.root" % (getPath(), options.out))
		except:
			print "Could not create symlink."

	else:  # local
		if not options.fast and len(conf["InputFiles"])>100:
			print "Warning: The full run as a single job will take a while.",
			print "Are you sure? [Y/n]"
			try:
				if raw_input() == "n":
					exit(0)
			except KeyboardInterrupt:
				exit(0)
		try:
			subprocess.call([options.base + "/artus", options.json])
		except KeyboardInterrupt:
			aborted = True
			print '\33[31m%s\033[0m' % "Artus run was aborted prematurely."

	# show message and optionally open root file
	if aborted:
		showMessage("Excalibur", "Artus run with config " + options.out + " aborted.")
	else:
		showMessage("Excalibur", "Artus run with config " + options.out + " done.")
	if options.root and not aborted:
		print "\nOpen output file in TBrowser:"
		try:
			subprocess.call(["root", "-l",
				"%s/%s.root" % (options.base, options.out),
				"%s/scripts/tbrowser.cxx" % options.base])
		except:
			pass


def getoptions(configdir='Configuration/python/', name='artus'):
	"""Set standard options and read command line arguments. """

	parser = argparse.ArgumentParser(
		description="%(prog)s is the main analysis program.",
		epilog="Have fun.")

	# config file
	parser.add_argument('cfg', metavar='cfg', type=str, nargs='?', default=None,
		help="config file (.py or .py.json)" +
			 " - path: cfg/artus and .py can be omitted. No config implies mc -f")

	# options
	parser.add_argument('-b', '--batch', action='store_true',
		help="run with grid-control")
	parser.add_argument('-c', '--config', action='store_true',
		help="produce json config only")
	parser.add_argument('-C', '--clean', action='store_true',
		help="delete old outputs but one with the same name")
	parser.add_argument('-d', '--delete', action='store_true',
		help="delete the latest output and jobs still running")
	parser.add_argument('-f', '--fast', type=int, nargs='*', default=None,
		help="limit number of input files. 3=files[-3:], 5 6=files[5:6].")
	parser.add_argument('-l', '--nologo', action='store_true',
		help="do not print the logo")
	parser.add_argument('-o', '--out', type=str, nargs=1, default=None,
		help="specify custom output name (default: config name)")
	parser.add_argument('-p', '--printconfig', action='store_true',
		help="print json config (long output)")
	parser.add_argument('-s', '--skip', type=int, nargs='+', default=None,
		help="skip events. 5=events[5,5+1], 5 3=events[5,5+3].")
	parser.add_argument('-v', '--verbose', action='store_true',
		help="verbosity")
	parser.add_argument('-w', '--work', type=str, nargs=1, default=None,
		help="specify custom work path (default from $EXCALIBUR_WORK variable")
	parser.add_argument('-r', '--root', action='store_true',
		help="open output file in ROOT TBrowser after completion")
	parser.add_argument('-R', '--resume', action='store_true',
		help="resume the grid-control run and hadd after interrupting it.")

	opt = parser.parse_args()
	opt.base = getPath()

	# derive config file name
	if opt.cfg is None:
		opt.cfg = 'mc'
		if not opt.fast and not opt.batch and not opt.config:
			opt.fast = [3]
	if '/' not in opt.cfg:
		opt.cfg = opt.base + configdir + opt.cfg
	if '.py' not in opt.cfg:
		opt.cfg += '.py'
	if not os.path.exists(opt.cfg):
		print "Config file", opt.cfg, "does not exist."
		exit(1)

	# derive json config file name
	opt.isjson = (opt.cfg[-8:] == '.py.json')
	opt.json = opt.cfg[:]
	if opt.isjson and opt.config:
		print "Json config alread created. Nothing to do."
		exit(1)
	if not opt.isjson:
		opt.json.replace("python", "data")
		opt.json += ".json"

	# derive omitted values for fast and skip
	if opt.fast == []:
		opt.fast = [3]
	if opt.fast and len(opt.fast) == 1:
		opt.fast = [-opt.fast[0], None]
	if opt.skip and len(opt.skip) == 1:
		opt.skip.append(1)

	# set paths for libraries and outputs
	if not opt.out:
		opt.out = opt.cfg[opt.cfg.rfind('/') + 1:opt.cfg.rfind('.py')]
	
	#opt.boost = getPath('BOOSTPATH')
	if not opt.work:
		opt.work = "empty" #getPath('EXCALIBUR_WORK', True) or getPath()
	opt.work += '/' + name + '/' + opt.out
	if not opt.resume and not opt.delete:
		opt.timestamp = time.strftime("_%Y-%m-%d_%H-%M")
	else:
		paths = glob.glob(opt.work + "_20*")
		paths.sort()
		try:
			opt.timestamp = paths[-1][-17:]
		except:
			print "No existing output directory available!"
			exit(1)
	opt.work += opt.timestamp + '/'

	if opt.verbose:
		print opt
	return opt


def getPath(variable=None, nofail=False):
	try:
		if not variable:
			path = os.environ["CMSSW_BASE"] + "/src/Artus/"
		else:
			path = os.environ[variable]
		return path
	except:
		print variable, "is not in shell variables:", os.environ.keys()
		print "Please source ini script and CMSSW!"
		if nofail:
			return None
		exit(1)


#TODO: the following functions are specific to a specific setup, to be discussed!
def writeJson(settings, filename):
	with open(filename, 'w') as f:
		json.dump(settings, f, sort_keys=True, indent=4)


def writeDBS(settings, nickname, filename):
	# ordering is important in the .dbs file format
	with open(filename, 'wb') as f:
		f.write("[" + nickname + "]\n")
		f.write("nickname = " + nickname + "\n")
		f.write("events = " + str(-len(settings['InputFiles'])) + "\n")
		f.write("prefix = " + os.path.split(settings['InputFiles'][0])[0] + "\n")
		for i in settings['InputFiles']:
			f.write(os.path.split(i)[1] + " = -1\n")


def copyFile(source, target, replace={}):
	with open(source) as f:
		text = f.read()
	for a, b in replace.items():
		text = text.replace(a, b)
	with open(target, 'wb') as f:
		f.write(text)
	return text


def createGridControlConfig(settings, filename, original=None, timestamp=''):
	if original is None:
		original = getPath() + '/cfg/artus/base/gc.conf'
	jobs = {
			'mc': 80,
			'data': 160,
	}
	fpj = len(settings['InputFiles']) / float(jobs.get(settings['InputType'], 70))
	fpj = int(fpj + 1)
	d = {
		'files per job = 100': 'files per job = ' + str(fpj),
		'@NICK@': settings["OutputPath"],
		'@TIMESTAMP@': timestamp,
		'$EXCALIBUR_BASE': getPath(),
		'$EXCALIBUR_WORK': getPath('EXCALIBUR_WORK'),
		'$BOOSTLIB': getPath('BOOSTLIB'),
	}
	text = copyFile(original, filename, d)
	# return the name of output files
	text = text[text.find("se output pattern =") + 19:]
	text = text[:text.find("\n")]
	text = text.replace("@MY_JOBID@", "*")
	return text.strip()


def createRunfile(configjson, filename='test.sh', original=None, workpath=None):
	if original is None:
		original = getPath() + '/cfg/artus/base/run-artus.sh'
	with open(original) as f:
		text = f.read()
	text = text.replace('cfg/artus/config.py.json', configjson)
	if workpath is not None:
		text = text.replace("$EXCALIBUR_BASE/cfg/artus/", workpath)
		text = text.replace("$EXCALIBUR_BASE/artus", workpath + "artus")
	text = text.replace('$EXCALIBUR_BASE', getPath())
	text = text.replace('$CMSSW_BASE', getPath('CMSSW_BASE'))
	with open(filename, 'wb') as f:
		f.write(text)


def showMessage(title, message):
	userpc = "%s@%s" % (getPath('USER'), getPath('USERPC'))
	iconpath = '/usr/users/dhaitz/excalibur/excal_small.jpg'
	try:
		if 'ekplx' in userpc:
			subprocess.call(['ssh', userpc,
				'DISPLAY=:0 notify-send "%s" "%s" -i %s' % (title, message, iconpath)])
	except:
		pass
	print message


def createFileList(files, fast=False):
		if type(files) == str:
			if "*" in files:
				print "Creating file list from", files
				files = glob.glob(files)
			else:
				files = [files]
		if not files:
			print "No input files found."
			exit(1)
		if fast:
			files = files[fast[0]:fast[1]]
		return files


def prepareWork(work, out, clean=False):
	"""ensure that the output path exists and delete old outputs optionally)

	to save your outputs simply rename them without timestamp
"""
	if work[-1] == '/':
		work = work[:-1]
	if "_20" in work:
		paths = glob.glob(work[:-17]+"_20*")
		paths.sort()
		if paths:
			paths.pop()
	else:
		paths = glob.glob(work + "_20*")
	if clean:
		for p in paths:
			if os.path.exists(p):
				print "removing", p
				shutil.rmtree(p)
	elif len(paths) > 1:
		print len(paths), "old output directories for this config. Clean-up recommended."
	print "Output directory:", work
	os.makedirs(work + "/work." + out)


def logo():
	return """ARTUS"""

if __name__ == "__main__":
	artus()
