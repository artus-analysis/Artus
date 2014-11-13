# -*- coding: utf-8 -*-

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import numpy as np
import ROOT


# def root2histo(histo, rootfile='', rebin=1):
# 	"""Convert a root histogram to the Histo class"""
# 	hst = Histo()
#
# 	if not hasattr(histo, 'ClassName'):
# 		log.critical(histo + " is no TObject. It could not be converted.")
# 		exit(1)
# 	# Detect if it is a histogram or a graph
# 	if histo.ClassName() == 'TH1D' or histo.ClassName() == 'TH1F' or \
# 			histo.ClassName() == 'TProfile':
# 		# histo is a 1D histogram, read it
# 		if rebin > 1:
# 			histo.Rebin(rebin)
# 		hst.source = rootfile
# 		hst.name = histo.GetName()
# 		hst.classname = histo.ClassName()
# 		hst.title = histo.GetTitle()
# 		hst.xlabel = histo.GetXaxis().GetTitle()
# 		hst.ylabel = histo.GetYaxis().GetTitle()
# 		hst.RMS = histo.GetRMS()
# 		hst.RMSerr = histo.GetRMSError()
# 		for i in range(1, histo.GetSize() - 1):
# 			hst.x.append(histo.GetBinLowEdge(i))
# 			hst.xc.append(histo.GetBinCenter(i))
# 			hst.y.append(histo.GetBinContent(i))
# 			hst.xerr.append(histo.GetBinWidth(i) / 2.0)
# 			hst.yerr.append(histo.GetBinError(i))
# 		#hst.x.append(histo.GetBinLowEdge(histo.GetSize() - 1))
# 		#hst.xc.append(histo.GetBinLowEdge(histo.GetSize() - 1))
# 		#hst.y.append(0.0)
# 		#hst.xerr.append(0.0)
# 		#hst.yerr.append(0.0)
# 		hst.mean = histo.GetMean()
# 		hst.meanerr = histo.GetMeanError()
# 	elif histo.ClassName() == 'TH2D' or histo.ClassName() == 'TH2F' or histo.ClassName() == 'TProfile2D':
# 		hst = Histo2D()
# 		histo.RebinX(rebin)
# 		histo.RebinY(rebin)
# 		hst.source = rootfile
# 		hst.name = histo.GetName()
# 		hst.classname = histo.ClassName()
# 		hst.title = histo.GetTitle()
# 		hst.xlabel = histo.GetXaxis().GetTitle()
# 		hst.ylabel = histo.GetYaxis().GetTitle()
# 		for x in range(1, histo.GetNbinsX() + 1):
# 			hst.x.append(histo.GetBinLowEdge(x))
# 			hst.xc.append(histo.GetBinCenter(x))
# 			hst.xerr.append(histo.GetBinWidth(x) / 2.0)
# 		a = np.zeros(shape=(histo.GetNbinsY(), histo.GetNbinsX()))
# 		hst.BinContents = np.ma.masked_equal(a, 0.0)
# 		for y in range(1, histo.GetNbinsY() + 1):
# 			for x in range(1, histo.GetNbinsX() + 1):
# 				if (histo.ClassName() != 'TProfile2D') or histo.GetBinEntries(histo.GetBin(x, y)) > 0:
# 					hst.BinContents[y - 1, x - 1] = histo.GetBinContent(x, y)
# 			hst.y.append(histo.GetYaxis().GetBinLowEdge(y))
# 			hst.yc.append(histo.GetYaxis().GetBinCenter(y))
# 			hst.yerr.append(histo.GetYaxis().GetBinWidth(y) / 2.0)
# 		hst.xborderhigh = hst.xc[-1] + hst.xerr[-1]
# 		hst.xborderlow = hst.xc[0] - hst.xerr[0]
# 		hst.yborderhigh = hst.yc[-1] + hst.yerr[-1]
# 		hst.yborderlow = hst.yc[0] - hst.yerr[0]
# 		hst.mean = histo.GetMean()
# 		hst.meanerr = histo.GetMeanError()
# 	elif histo.ClassName() == 'TGraphErrors' or histo.ClassName() == 'TGraph':
# 		# histo is a graph, read it
# 		hst.source = rootfile
# 		hst.name = histo.GetName()
# 		hst.classname = histo.ClassName()
# 		hst.title = histo.GetTitle()
# 		hst.xlabel = histo.GetXaxis().GetTitle()
# 		hst.ylabel = histo.GetYaxis().GetTitle()
# 		a = ROOT.Double(0.0)
# 		b = ROOT.Double(0.0)
# 		for i in range(0, histo.GetN()):
# 			histo.GetPoint(i, a, b)
# 			x = float(a)
# 			y = float(b)
# 			xerr = histo.GetErrorX(i)
# 			yerr = histo.GetErrorY(i)
# 			hst.x.append(x)
# 			hst.xc.append(x)
# 			hst.y.append(y)
# 			hst.xerr.append(xerr if xerr > 0.0 else 0.0)
# 			hst.yerr.append(yerr if yerr > 0.0 else 0.0)
# 	elif histo.ClassName() == 'TGraphAsymmErrors':
# 		# histo is a graph, read it
# 		hst.source = rootfile
# 		hst.name = histo.GetName()
# 		hst.classname = histo.ClassName()
# 		hst.title = histo.GetTitle()
# 		hst.xlabel = histo.GetXaxis().GetTitle()
# 		hst.ylabel = histo.GetYaxis().GetTitle()
# 		a = ROOT.Double(0.0)
# 		b = ROOT.Double(0.0)
# 		hst.xerr = [[],[]]
# 		hst.yerr = [[],[]]
# 		for i in range(0, histo.GetN()):
# 			histo.GetPoint(i, a, b)
# 			x = float(a)
# 			y = float(b)
# 			xerr_low = histo.GetErrorXlow(i)
# 			xerr_high = histo.GetErrorXhigh(i)
# 			yerr_low = histo.GetErrorYlow(i)
# 			yerr_high = histo.GetErrorYhigh(i)
# 			hst.x.append(x)
# 			hst.xc.append(x)
# 			hst.y.append(y)
# 			hst.xerr[0].append(xerr_low)
# 			hst.xerr[1].append(xerr_high)
# 			hst.yerr[0].append(yerr_low)
# 			hst.yerr[1].append(yerr_high)
# 	else:
# 		# histo is of unknown type
# 		log.critical("The object '" + str(histo) + "' is no histogram, no graph and no profile! It could not be converted!")
# 		exit(1)
# 	return hst

class MplHisto1D:
	""" Convenient representation of 1D Root histogram to be used for matplotlib."""

	def __init__(self, roothisto):
		if not roothisto.ClassName() in ['TH1D', 'TH1F', 'TProfile']:
			raise TypeError('No valid TH1D, TH1F or TProfile passed.')
		self.name = roothisto.GetName()
		self.roothisto = roothisto
		# self.classname = roothisto.ClassName()
		self.title = roothisto.GetTitle()
		self.xlabel = roothisto.GetXaxis().GetTitle()
		self.ylabel = roothisto.GetYaxis().GetTitle()
		# Number of bins without underflow/overflow bin
		self.size = roothisto.GetNbinsX()

		# bin center
		self.x = np.array([roothisto.GetXaxis().GetBinCenter(i) for i in xrange(1, self.size +1)])
		# lower bin edge
		self.xl = np.array([roothisto.GetXaxis().GetBinLowEdge(i) for i in xrange(1, self.size +1)])
		# upper bin edge
		self.xu = np.array([roothisto.GetXaxis().GetBinUpEdge(i) for i in xrange(1, self.size +1)])
		# bin content
		self.y = np.array([roothisto.GetBinContent(i) for i in xrange(1, self.size +1)])
		# bin content
		self.yerr = np.array([roothisto.GetBinError(i) for i in xrange(1, self.size +1)])
		# lower bin error
		self.yerrl = np.array([roothisto.GetBinErrorLow(i) for i in xrange(1, self.size +1)])
		# upper bin error
		self.yerru = np.array([roothisto.GetBinErrorUp(i) for i in xrange(1, self.size +1)])

	@property
	def xerr(self):
		return 0.5 * self.xbinwidth

	@property
	def xerrl(self):
		return self.x - self.xl

	@property
	def xerru(self):
		return self.xu - self.x

	@property
	def xbinwidth(self):
		return self.xu -self.xl

	@property
	def xbinedges(self):
		return np.concatenate((self.xl, self.xu[-1:]))

	@property
	def yl(self):
		return self.y - self.yerrl

	@property
	def yu(self):
		return self.y + self.yerru


#class Histo2D(Histo):
#	def __init__(self):
#		Histo.__init__(self)
#		self.yc = []
#		self.BinContents = []
#		self.xborderhigh = 0.0
#		self.yborderhigh = 0.0
#		self.xborderlow = 0.0
#		self.yborderlow = 0.0
