#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import time
import os
import argparse

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	verbose_comparison = False
	files = []
	trees = []
	stopWatch()
	parser = argparse.ArgumentParser(description='Event matching tool.')
	parser.add_argument('input_files', metavar='file.root', type=str, nargs='+', default=None,
		help="two or three input files that should be matched")
	parser.add_argument('-n', '--nicks', type=str, nargs='+', default=list(["1", "2", "3"]),
		help="nicks of root trees")
	parser.add_argument('-o', '--common', action='store_true',
		help="only write common events to output file")
	parser.add_argument('-t', '--tree', type=str, nargs='+', default=list(["finalcuts_AK5PFJetsCHSL1L2L3Res/ntuple"]),
		help="tree names (default: %(default)s)")
	parser.add_argument('-f', '--out', type=str, default="eventmatching.root",
		help="output file name (default: %(default)s)")
	parser.add_argument('-j', '--json-output-dir', type=str, default=None,
		help="output directory for JSON configs. (default: %(default)s)")
	parser.add_argument('-v', '--verbose', action='store_true',
		help="verbose comparison of common trees")
	parser.add_argument('-i', '--ignore', type=str, nargs='+', default=None,
		help="List of quantities to be ignored in the verbose comparison")
	parser.add_argument('-d', '--dictionary', type=str, nargs='+', default=None,
		help="Translation of branchnames between the trees in the form branchname1:branchname2")

	args = parser.parse_args()
	if len(args.input_files) < 2:
		sys.exit("At least two ROOT files are required!")
	# Expand tree list to length of input files list using the last item in tree list
	if len(args.tree) < len(args.input_files):
		args.tree[len(args.tree):] = [args.tree[-1]]*(len(args.input_files)-len(args.tree))
	# Expand nicks list to length of input files list
	if len(args.nicks) < len(args.input_files):
		sys.exit("%d nicks are required!") % len(args.input_files)

	print "Read %d files:" % len(args.input_files)
	for i in range(0, len(args.input_files)):
		print "  File%2d: %s (%s)" % (i+1, args.input_files[i], args.tree[i])
		files.append(ROOT.TFile(args.input_files[i]))
		trees.append(files[-1].Get(args.tree[i]))
	stopWatch()

	print "\nGet list of runs, lumis, events:"
	lists = []
	for tree in trees:
		lst = getRunLumiEvent(tree)
		lists.append(sorted(lst))
	stopWatch()

	print "\nCompare lists:"
	com12, o1, o2 = compareLists(lists[0], lists[1])
	if len(args.input_files) == 3:
		com123, ocom12, o3 = compareLists(com12, lists[2])
		# Calculate new only lists if requested
		if not args.common:
			com13, tmp, tmp = compareLists(lists[0], lists[2])
			com23, tmp, tmp = compareLists(lists[1], lists[2])
			tmp, tmp, o2 = compareLists(com13, lists[1])
			tmp, tmp, o1 = compareLists(com23, lists[0])
	stopWatch()
	
	print "\nCopy to output trees:"
	fout = ROOT.TFile(args.out, "RECREATE")
	if len(args.input_files) == 3:
		c1 = cpTree(com123, trees[0], "common" + args.nicks[0], 0, jsonConfigsDir=args.json_output_dir)
		stopWatch()
		c2 = cpTree(com123, trees[1], "common" + args.nicks[1], 1, jsonConfigsDir=args.json_output_dir)
		stopWatch()
		c3 = cpTree(com123, trees[2], "common" + args.nicks[2], 2, jsonConfigsDir=args.json_output_dir)
		stopWatch()
		if not args.common:
			cpTree(o1, trees[0], "only" + args.nicks[0], 0, jsonConfigsDir=args.json_output_dir)
			stopWatch()
			cpTree(o2, trees[1], "only" + args.nicks[1], 0, jsonConfigsDir=args.json_output_dir)  # treeIndex 0 is correct
			stopWatch()
			cpTree(o3, trees[2], "only" + args.nicks[2], 0, jsonConfigsDir=args.json_output_dir)  # treeIndex 0 is correct
			stopWatch()
	else:
		c1 = cpTree(com12, trees[0], "common" + args.nicks[0], 0, jsonConfigsDir=args.json_output_dir)
		stopWatch()
		c2 = cpTree(com12, trees[1], "common" + args.nicks[1], 1, jsonConfigsDir=args.json_output_dir)
		stopWatch()
		if not args.common:
			cpTree(o1, trees[0], "only" + args.nicks[0], 0, jsonConfigsDir=args.json_output_dir)
			stopWatch()
			cpTree(o2, trees[1], "only" + args.nicks[1], 0, jsonConfigsDir=args.json_output_dir)  # treeIndex 0 is correct
			stopWatch()

	fout.Write()
	print "\nTrees written to file", fout.GetName()
	
	if args.verbose:
		translation = {}
		if args.dictionary:
			for d in args.dictionary:
				k, v = d.split(':', 1)
				translation[k] = v
		print "\nCompare common trees 1 and 2:"
		compareTrees(c1, c2, args.ignore, translation)
		if len(args.input_files) == 3:
			print "\nCompare common trees 1 and 3:"
			compareTrees(c1, c3, args.ignore)
			print "\nCompare common trees 2 and 3:"
			compareTrees(c2, c3, args.ignore)

	stopWatch(overall=True)
	
def stopWatch(n=[], overall=False):
	"""print the time needed since last call"""
	n.append(time.time())
	if len(n) > 1:
		print "  -- Last step took: %1.3f s" % (n[-1] - n[-2])
	if overall:
		print "  -- Overall time:   %1.3f s" % (n[-1] - n[0])

def getRunLumiEvent(tree):
	"""get list of (run, lumi, event, entry index) from a tree"""
	result = []
	nevt = tree.GetEntries()
	eventbranch = 'event'
	for name in ['event', 'evt', 'eventnr']:
		if not hasattr(tree, eventbranch) and hasattr(tree, name):
			eventbranch = name

	for i in xrange(nevt):
		tree.GetEntry(i)
		result.append((int(tree.run), int(tree.lumi), int(getattr(tree, eventbranch)), i))
		if i % 1000 == 0:
			print "\r  %7d/%d" % (i, nevt),
			sys.stdout.flush() 
	print "\r  %7d/%d" % (i+1, nevt)
	return result

def compareLists(list1, list2):
	"""Compare two lists of (run, lumi, evt, index) and sort them into
	   three lists: common events, events only in list 1 and events only
	   in list 2"""
	n = 0
	m = 0
	common = []
	only1 = []
	only2 = []
	while n < len(list1) and m < len(list2):
		if n % 1000 == 0 or m % 1000 == 0 and m != 0 and n!=0:
			print "\r  tree1:%7d/%d, tree2:%7d/%d -> common:%7d, tree1: %5d, tree2: %5d" % (
				n, len(list1), m, len(list2), len(common), len(only1), len(only2)),
		if list1[n][:3] == list2[m][:3]:
			common.append(list1[n] + (list2[m][3],))  # the common list has two indices
			n += 1
			m += 1
		elif list1[n][:3] < list2[m][:3]:
			only1.append(list1[n])
			n += 1
		elif list1[n][:3] > list2[m][:3]:
			only2.append(list2[m])
			m += 1
		else:
			print "What?"
	print "\r  tree1:%7d/%d, tree2:%7d/%d -> common:%7d, tree1: %5d, tree2: %5d" % (
				n, len(list1), m, len(list2), len(common), len(only1), len(only2))
	return common, only1, only2


def cpTree(eventList, tree, name, treeIndex=0, deactivate=None, jsonConfigsDir=None):
	"""Copy the events in eventList[i][3+treeIndex] from tree to a new tree of name 'name'"""
	if deactivate:
		for q in quantities:
			tree.SetBranchStatus(q, 0)
	outputTree = tree.CloneTree(0)
	outputTree.SetName(name)
	outputTree.SetTitle(tree.GetTitle() + "_" + name)
	branches = [b.GetName() for b in tree.GetListOfBranches()]
	print "  tree %r (%d branches, %d entries) to %r (%d branches, %d entries)" % (
		tree.GetName(), len(tree.GetListOfBranches()), tree.GetEntries(),
		name, len(outputTree.GetListOfBranches()), outputTree.GetEntries()),
	assert outputTree.GetEntries() == 0
	sys.stdout.flush()
	
	
	nevt = len(eventList)
	for i, evt in zip(range(nevt), eventList):
		# evt is (run, lumi, event, index in tree1, index in tree2)
		tree.GetEntry(evt[3 + treeIndex])
		if 'TNtuple' in str(type(tree)):
			outputTree.Fill(evt[3 + treeIndex])
		else:
			outputTree.Fill()
		if i % 100 == 0:
			print "\r  %7d/%d" % (i, nevt),
			sys.stdout.flush()

	outputTree.Write(name)
	print "\r  tree %r (%d branches, %d entries) to %r (%d branches, %d entries)" % (
		tree.GetName(), len(tree.GetListOfBranches()), tree.GetEntries(),
		name, len(outputTree.GetListOfBranches()), outputTree.GetEntries())
	
	if not jsonConfigsDir is None:
		jsonConfigFileName = os.path.join(jsonConfigsDir, name+".json")
		with open(jsonConfigFileName, "w") as jsonConfigFile:
			jsonConfigFile.write("{\n\t\"EventWhitelist\" : [\n\t\t" + (",\n\t\t".join([str(entry[2]) for entry in eventList])) + "\n\t]\n}")
		print "  EventWhitelist saved in JSON config file \"%s\"." % jsonConfigFileName
	
	return outputTree


def compareTrees(tree1, tree2, ignored=None, nameconversion=None):
	if not nameconversion:
		nameconversion = {}		# ex2  :  ex1
	branches1orig = [b.GetName() for b in tree1.GetListOfBranches()] # branches of tree1
	branches2orig = [b.GetName() for b in tree2.GetListOfBranches()] # branches of tree2
	branches1used = [] # branches of tree1 that are in sync with tree2
	branches2used = []
	branches1only = []
	branches2only = [b for b in branches1orig]

	for b1 in branches1orig:
		b2 = nameconversion.get(b1, b1)
		if b2 in branches2orig:
			branches1used.append(b1)
			branches2used.append(b2)
			branches2only.remove(b2)
		else:
			branches1only.append(b1)
	print "  * Branch comparison:"
	print "  %4d branches only in tree1: %s" % (len(branches1only), ", ".join(branches1only))
	print "  %4d branches only in tree2: %s" % (len(branches2only), ", ".join(branches2only))
	print "  %4d common branches" % len(branches1used)
	assert len(branches1used) == len(branches2used)

	tree1.AddFriend(tree2.GetName())
	for i in xrange(tree1.GetEntries()):
		if i % 100 == 0:
			print "\rEntry", i,
		tree1.GetEntry(i)
		tree2.GetEntry(i)
		for b1, b2 in zip(branches1used, branches2used):
			v1 = getattr(tree1, b1)
			v2 = getattr(tree2, b2)
			if b1 == 'eventnr':
				v1 = int(tree1.eventnr1) * 1000000 + tree1.eventnr2
			if abs(v1 - v2) > 1e-3 and b1 not in ignored:
				# print only eta differences if jets differ completely:
				print "\rIn entry %7d, %s differs: %s = %s, %s = %s" % (i, b2, b1, v1, b2, v2)
	print "\rEntry", i


main()
