# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import numpy as np
import ROOT

class MplGraph:

	def __init__(self, rootgraph):
		if not isinstance(rootgraph, ROOT.TGraph): 
			raise TypeError(str(rootgraph.ClassName()) + ' does not inherit from TGraph.')
		self.name = rootgraph.GetName()
		self.rootgraph = rootgraph
		# self.classname = roothisto.ClassName()
		self.title = rootgraph.GetTitle()
		self.xlabel = rootgraph.GetXaxis().GetTitle()
		self.ylabel = rootgraph.GetYaxis().GetTitle()
		# Number of bins without underflow/overflow bin
		self.x = np.zeros((rootgraph.GetN()))
		self.y = np.zeros((rootgraph.GetN()))

		for i in xrange(rootgraph.GetN()):
			tmpX, tmpY = ROOT.Double(0), ROOT.Double(0)
			rootgraph.GetPoint(i, tmpX, tmpY)
			self.x[i] = tmpX
			self.y[i] = tmpY
		self.xerr = np.array([rootgraph.GetErrorX(i) for i in xrange(rootgraph.GetN())])
		self.xerrl = np.array([rootgraph.GetErrorXlow(i) for i in xrange(rootgraph.GetN())])
		self.xerru = np.array([rootgraph.GetErrorXhigh(i) for i in xrange(rootgraph.GetN())])
		self.yerr = np.array([rootgraph.GetErrorY(i) for i in xrange(rootgraph.GetN())])
		self.yerrl = np.array([rootgraph.GetErrorYlow(i) for i in xrange(rootgraph.GetN())])
		self.yerru = np.array([rootgraph.GetErrorYhigh(i) for i in xrange(rootgraph.GetN())])

	@property
	def xbinedges(self):
		return np.concatenate((self.xl, self.xu[-1:]))

	@property
	def xl(self):
		return self.x - self.xerrl

	@property
	def xu(self):
		return self.x + self.xerru

	@property
	def yl(self):
		return self.y - self.yerrl

	@property
	def yu(self):
		return self.y + self.yerru


class MplHisto(object):
	"""Simple representation of 1D or 2D Root histogram to be used for matplotlib plotting."""

	def __init__(self, roothisto):
		if not roothisto.ClassName() in ['TH1D', 'TH1F', 'TProfile', 'TH2D', 'TH2F', 'TProfile2D']:
			raise TypeError('No valid TH1D TH1F TProfile TH2D TH2F or TProfile2D passed.')

		self.name = roothisto.GetName()
		self.roothisto = roothisto
		# self.classname = roothisto.ClassName()
		self.title = roothisto.GetTitle()
		self.xlabel = roothisto.GetXaxis().GetTitle()
		self.ylabel = roothisto.GetYaxis().GetTitle()

		# bin center
		self.x = np.array([roothisto.GetXaxis().GetBinCenter(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
		# lower bin edge
		self.xl = np.array([roothisto.GetXaxis().GetBinLowEdge(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
		# upper bin edge
		self.xu = np.array([roothisto.GetXaxis().GetBinUpEdge(i) for i in xrange(1, roothisto.GetNbinsX() +1)])

		if roothisto.ClassName() in ['TH1D', 'TH1F', 'TProfile']:
			self.dimension = 1
			# bin content
			self.bincontents = np.array([roothisto.GetBinContent(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
			# bin content
			self.binerr = np.array([roothisto.GetBinError(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
			# lower bin error
			self.binerrl = np.array([roothisto.GetBinErrorLow(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
			# upper bin error
			self.binerru = np.array([roothisto.GetBinErrorUp(i) for i in xrange(1, roothisto.GetNbinsX() +1)])
		elif roothisto.ClassName() in ['TH2D', 'TH2F', 'TProfile2D']:
			self.dimension = 2
			self.y = np.array([roothisto.GetYaxis().GetBinCenter(i) for i in xrange(1, roothisto.GetNbinsY() +1)])
			# lower bin edge
			self.yl = np.array([roothisto.GetYaxis().GetBinLowEdge(i) for i in xrange(1, roothisto.GetNbinsY() +1)])
			# upper bin edge
			self.yu = np.array([roothisto.GetYaxis().GetBinUpEdge(i) for i in xrange(1, roothisto.GetNbinsY() +1)])

			self.bincontents = np.zeros((roothisto.GetNbinsY(), roothisto.GetNbinsX()))
			if roothisto.ClassName() == 'TProfile2D':
				self.bincontents = np.ma.masked_equal(self.bincontents, 0.0)
			self.binerrl = np.zeros((roothisto.GetNbinsY(), roothisto.GetNbinsX()))
			self.binerru = np.zeros((roothisto.GetNbinsY(), roothisto.GetNbinsX()))
			for y in xrange(1, roothisto.GetNbinsY() + 1):
				for x in xrange(1, roothisto.GetNbinsX() + 1):
					if (roothisto.ClassName() != 'TProfile2D') or roothisto.GetBinEntries(roothisto.GetBin(x, y)) > 0:
						self.bincontents[y - 1, x - 1] = roothisto.GetBinContent(x, y)
						self.binerrl[y - 1, x - 1] = roothisto.GetBinErrorLow(x, y)
						self.binerru[y - 1, x - 1] = roothisto.GetBinErrorUp(x, y)

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
	def yerr(self):
		return 0.5 * self.ybinwidth

	@property
	def yerrl(self):
		return self.y - self.yl

	@property
	def yerru(self):
		return self.yu - self.y

	@property
	def ybinwidth(self):
		return self.yu - self.yl

	@property
	def ybinedges(self):
		return np.concatenate((self.yl, self.yu[-1:]))

	@property
	def bincontentsl(self):
		return self.bincontents - self.binerrorsl

	@property
	def bincontentsu(self):
		return self.bincontents + self.binerrorsu
