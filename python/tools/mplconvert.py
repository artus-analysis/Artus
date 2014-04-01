# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import time
import numpy as np
import ROOT


def root2histo(histo, rootfile='', rebin=1):
	"""Convert a root histogram to the Histo class"""
	hst = Histo()

	if not hasattr(histo, 'ClassName'):
		print histo, "is no TObject. It could not be converted."
		exit(0)
	# Detect if it is a histogram or a graph
	if histo.ClassName() == 'TH1D' or histo.ClassName() == 'TH1F' or \
			histo.ClassName() == 'TProfile':
		# histo is a 1D histogram, read it
		if rebin > 1:
			histo.Rebin(rebin)
		hst.source = rootfile
		hst.name = histo.GetName()
		hst.classname = histo.ClassName()
		hst.title = histo.GetTitle()
		hst.xlabel = histo.GetXaxis().GetTitle()
		hst.ylabel = histo.GetYaxis().GetTitle()
		hst.RMS = histo.GetRMS()
		hst.RMSerr = histo.GetRMSError()
		for i in range(1, histo.GetSize() - 1):
			hst.x.append(histo.GetBinLowEdge(i))
			hst.xc.append(histo.GetBinCenter(i))
			hst.y.append(histo.GetBinContent(i))
			hst.xerr.append(histo.GetBinWidth(i) / 2.0)
			hst.yerr.append(histo.GetBinError(i))
		#hst.x.append(histo.GetBinLowEdge(histo.GetSize() - 1))
		#hst.xc.append(histo.GetBinLowEdge(histo.GetSize() - 1))
		#hst.y.append(0.0)
		#hst.xerr.append(0.0)
		#hst.yerr.append(0.0)
		hst.mean = histo.GetMean()
		hst.meanerr = histo.GetMeanError()
	elif histo.ClassName() == 'TH2D' or histo.ClassName() == 'TH2F' or histo.ClassName() == 'TProfile2D':
		hst = Histo2D()
		histo.RebinX(rebin)
		histo.RebinY(rebin)
		hst.source = rootfile
		hst.name = histo.GetName()
		hst.title = histo.GetTitle()
		hst.xlabel = histo.GetXaxis().GetTitle()
		hst.ylabel = histo.GetYaxis().GetTitle()
		for x in range(1, histo.GetNbinsX() + 1):
			hst.x.append(histo.GetBinLowEdge(x))
			hst.xc.append(histo.GetBinCenter(x))
			hst.xerr.append(histo.GetBinWidth(x) / 2.0)
		a = np.zeros(shape=(histo.GetNbinsY(), histo.GetNbinsX()))
		hst.BinContents = np.ma.masked_equal(a, 0.0)
		for y in range(1, histo.GetNbinsY() + 1):
			for x in range(1, histo.GetNbinsX() + 1):
				if (histo.ClassName() != 'TProfile2D') or histo.GetBinEntries(histo.GetBin(x, y)) > 0:
					hst.BinContents[y - 1, x - 1] = histo.GetBinContent(x, y)
			hst.y.append(histo.GetYaxis().GetBinLowEdge(y))
			hst.yc.append(histo.GetYaxis().GetBinCenter(y))
			hst.yerr.append(histo.GetYaxis().GetBinWidth(y) / 2.0)
		hst.xborderhigh = hst.xc[-1] + hst.xerr[-1]
		hst.xborderlow = hst.xc[0] - hst.xerr[0]
		hst.yborderhigh = hst.yc[-1] + hst.yerr[-1]
		hst.yborderlow = hst.yc[0] - hst.yerr[0]
		hst.mean = histo.GetMean()
		hst.meanerr = histo.GetMeanError()
	elif histo.ClassName() == 'TGraphErrors' or histo.ClassName() == 'TGraph':
		# histo is a graph, read it
		hst.source = rootfile
		hst.name = histo.GetName()
		hst.title = histo.GetTitle()
		hst.xlabel = histo.GetXaxis().GetTitle()
		hst.ylabel = histo.GetYaxis().GetTitle()
		a = ROOT.Double(0.0)
		b = ROOT.Double(0.0)
		for i in range(0, histo.GetN()):
			histo.GetPoint(i, a, b)
			x = float(a)
			y = float(b)
			xerr = histo.GetErrorX(i)
			yerr = histo.GetErrorY(i)
			hst.x.append(x)
			hst.xc.append(x)
			hst.y.append(y)
			hst.xerr.append(xerr if xerr > 0.0 else 0.0)
			hst.yerr.append(yerr if yerr > 0.0 else 0.0)
	else:
		# histo is of unknown type
		print "The object '" + str(histo) + "' is no histogram, no graph and no profile!",
		print "It could not be converted."
		exit(0)
	return hst


# TODO: remove functions modifying the histograms, since these functions are better checked in ROOT histogram classes
# TODO: I.e. Sum of error squares missing for correct modification of histograms
class Histo:
	"""Reduced Histogramm or Graph

	Self-defined histogram class
	constructor needed: (), (roothisto), (x,y,yerr)
	"""
	def __init__(self):
		self.source = ""
		self.path = ""
		self.name = ""
		self.classname = ""
		self.title = ""
		self.xlabel = "x"
		self.ylabel = "y"
		self.x = []
		self.xc = []
		self.y = []
		self.xerr = []
		self.yerr = []  # TODO yel, yeh wenn unterschiedlich...
		self.mean = 0.0
		self.meanerr = 0.0
		self.RMS = 0.0
		self.RMSerr = 0.0

	def __len__(self):
		return len(self.y)

	def scale(self, factor):
		for i in range(len(self.y)):
			self.y[i] *= factor
			self.yerr[i] *= factor

	def ysum(self):
		return sum(self.y)

	def __sum__(self):
		return sum(self.y)

	def __min__(self):
		return min(self.y)

	def __max__(self):
		return max(self.y)

	def ymax(self):
		return max(self.y)

	def ymin(self):
		return min(self.y)

	def norm(self):
		return 1.0 / sum(self.y)

	def normalize(self, factor=1.0):
		self.scale(factor * self.norm())
		return self

	def append(self, x, xc, y, yerr=0, xerr=0):
		if xc == True:
			xc = x
		self.x.append(x)
		self.xc.append(xc)
		self.y.append(y)
		self.xerr.append(xerr)
		self.yerr.append(yerr)

	def dropbin(self, number):
		self.x.pop(number)
		self.xc.pop(number)
		self.y.pop(number)
		self.xerr.pop(number)
		self.yerr.pop(number)

	def __div__(self, other):
		#if 0 in other.y:
		#	print "Division by zero!"
		#	return None
		if len(self) != len(other):
			print "Histos of different lengths! The shorter is taken."
		res = Histo()
		res.x = [0.5 * (a + b) for a, b in zip(self.x, other.x)]
		res.xc = [0.5 * (a + b) for a, b in zip(self.xc, other.xc)]
		res.y = [a / b if b != 0 else 0. for a, b in zip(self.y, other.y)]
		res.xerr = [0.5 * (abs(da) + abs(db)) for da, db in zip(self.xerr, other.xerr)]
		res.yerr = [abs(da / b) + abs(db * a / b / b) if b != 0 else 0. for a, da, b, db in zip(self.y, self.yerr, other.y, other.yerr)]
		return res

	def read(self, filename):
		"""Read the histogram from a text file

		"""
		f = file(filename, 'r')
		self.__init__()
		while True:
			line = f.readline()
			if not line:
				break
			if '#' in line:
				if 'Histogram' in line:
					self.name = getValue(line, ':')
				elif 'Path' in line:
					self.path = getValue(line, ':')
				elif 'file' in line:
					self.source = getValue(line, ':')
				elif 'lumi' in line:
					self.lumi = getValue(line, ':')
				elif 'x label' in line:
					self.xlabel = getValue(line, ':')
				elif 'y label' in line:
					self.xlabel = getValue(line, ':')
				elif 'Title' in line:
					self.title = getValue(line, ':')
			else:
				values = line.split()
				self.x.append(float(values[1]))
				if len(values) > 5:
					self.xc.append(float(values[2]))
					self.y.append(float(values[3]))
					self.yerr.append(float(values[5]))
				elif len(values) > 2:
					self.xc.append(float(values[1]))
					self.y.append(float(values[2]))
					self.yerr.append(0.0)

	def write(self, filename='.txt'):
		"""Write the histogram to a text file"""
		if filename == '.txt':
			filename = 'out/' + self.name + '.txt'
		f = file(filename, 'w')
		f.write(str(self))
		f.close()

	def __str__(self):
		"""Show the data of the histogram"""
		text = "#Histogram: " + self.name
		text += "\n#Path:	  " + self.path
		text += "\n#From file: " + self.source
		text += time.strftime("\n#Date/time: %a, %d %b %Y %H:%M:%S", time.localtime())
		text += "\n#Norm:	  " + str(self.norm())
		text += "\n#Sum:	   " + str(self.ysum())
		text += "\n#Maximum:   " + str(self.ymax())
		text += "\n#x label:   " + self.xlabel
		text += "\n#y label:   " + self.ylabel
		text += "\n#title:	 " + self.title
		text += "\n#energy:	7"  # in TeV
		text += "\n#  i	 x		xmid	  y			   ynorm"
		text += "		   yerr			ynormerr\n"
		if len(self.y) != len(self.x):
			print "This will fail because x and y have not the same length."
		if self.xc == []:
			self.xc = self.x
		for i in range(len(self.y)):
			text += '%4d %8.2f %8.2f %15.8f %15.8f %15.8f %15.8f\n' % (
					i, self.x[i], self.xc[i], self.y[i], self.y[i] * self.norm(),
					self.yerr[i], self.yerr[i] * self.norm())
		return text

	def dump(self, filename='.dat'):
		"""Write the histogram to a data file"""
		if filename == '.dat':
			filename = 'out/dat/' + self.name + '.dat'
		try:
			f = file(filename, 'w')
			pickle.dump(self, f)
		except:
			print "File " + filename + " could not be written."
		finally:
			f.close()

	def load(self, filename):
		"""Write the histogram to a data file"""
		f = file(filename)
		self = pickle.load(f)

	def draw(self, filename="preview.png"):
		import matplotlib.pyplot as plt
		fig = plt.figure()
		ax = fig.add_subplot(111)
		if len(self.y) == len(self.yerr):
			ax.errorbar(self.x, self.y, self.yerr)
		else:
			ax.plot(self.x, self.y)
		fig.savefig(filename)


def getValue(line, key):
	value = line[line.rfind(key) + len(key):line.rfind('\n')]
	value = str.strip(value)
	try:
		val = float(value)
		return val
	except:
		return value


class Histo2D(Histo):
	def __init__(self):
		Histo.__init__(self)
		self.yc = []
		self.BinContents = []
		self.xborderhigh = 0.0
		self.yborderhigh = 0.0
		self.xborderlow = 0.0
		self.yborderlow = 0.0

	def scale(self, factor):
		BinContentsNew = []
		for l in self.BinContents:
			BinContentsNew.append(map(lambda item: item * factor, l))
		self.BinContents = BinContentsNew

	def binsum(self):
		return sum([sum(a) for a in self.BinContents])

	def maxBin(self):
		a = max([max(l) for l in self.BinContents])
		return a


def histo2root(plot):
	"""Convert a Histo object to a root histogram (TH1D)"""
	title = plot.title + ";"  # + plot.xlabel() + ";" + plot.ylabel()
	print len(plot), title
	print min(plot.x), max(plot.x)
	th1d = ROOT.TH1D(plot.name, title, len(plot), min(plot.x), max(plot.x))

	if not hasattr(plot, 'dropbin'):
		print histo, "is no Histo. It could not be converted."
		exit(0)
	for i in range(0, len(plot)):
			th1d.SetBinContent(i + 1, plot.y[i])
			th1d.SetBinError(i + 1, plot.yerr[i])
			#th1d.SetBin(i, plot.x[i])
			#th1d.SetBinWidth(i, plot.xerr[i]/2)
			#th1d.SetBinCenter(i, plot.xc[i])
	return th1d


def writePlotToRootfile(plot, filename, plotname=None):
	if hasattr(plot, 'dropbin'):
		histo = histo2root(plot)
		print "File", filename
		print "plot", plotname
		f = ROOT.TFile("file.root", "RECREATE")
		histo.Write()
		f.Close()
