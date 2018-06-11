# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT



def set_default_root_style():

	default_root_style =  ROOT.TStyle("default_root_style", "Default style for HarryPlotter ROOT plots")

	# Canvas
	default_root_style.SetCanvasColor(0)
	default_root_style.SetCanvasBorderSize(10)
	default_root_style.SetCanvasBorderMode(0)
	default_root_style.SetCanvasDefH(700)
	default_root_style.SetCanvasDefW(700)
	default_root_style.SetCanvasDefX(100)
	default_root_style.SetCanvasDefY(100)

	# color palette for 2D temperature plots
	#default_root_style.SetPalette(1, 0)
	
	# Pads
	default_root_style.SetPadColor(0)
	default_root_style.SetPadBorderSize(10)
	default_root_style.SetPadBorderMode(0)
	default_root_style.SetPadBottomMargin(0.13)
	default_root_style.SetPadTopMargin(0.08)
	default_root_style.SetPadLeftMargin(0.15)
	default_root_style.SetPadRightMargin(0.05)
	default_root_style.SetPadGridX(0)
	default_root_style.SetPadGridY(0)
	default_root_style.SetPadTickX(1)
	default_root_style.SetPadTickY(1)

	# Frames
	default_root_style.SetLineWidth(1)
	default_root_style.SetFrameFillStyle(0)
	default_root_style.SetFrameFillColor(0)
	default_root_style.SetFrameLineColor(1)
	default_root_style.SetFrameLineStyle(0)
	default_root_style.SetFrameLineWidth(1)
	default_root_style.SetFrameBorderSize(10)
	default_root_style.SetFrameBorderMode(0)

	# Histograms
	default_root_style.SetHistFillColor(2)
	default_root_style.SetHistFillStyle(0)
	default_root_style.SetHistLineColor(1)
	default_root_style.SetHistLineStyle(0)
	default_root_style.SetHistLineWidth(3)
	default_root_style.SetNdivisions(505, "X")

	# Functions
	default_root_style.SetFuncColor(1)
	default_root_style.SetFuncStyle(0)
	default_root_style.SetFuncWidth(2)

	# Various
	default_root_style.SetMarkerStyle(20)
	default_root_style.SetMarkerColor(ROOT.kBlack)
	default_root_style.SetMarkerSize(1.1)

	default_root_style.SetTitleBorderSize(0)
	default_root_style.SetTitleFillColor(0)
	default_root_style.SetTitleX(0.2)

	default_root_style.SetTitleSize(0.055, "X")
	default_root_style.SetTitleOffset(1.200, "X")
	default_root_style.SetLabelOffset(0.005, "X")
	default_root_style.SetLabelSize(0.040, "X")
	default_root_style.SetLabelFont(42, "X")

	default_root_style.SetStripDecimals(False)
	default_root_style.SetLineStyleString(11, "20 10")

	default_root_style.SetTitleSize(0.055, "Y")
	default_root_style.SetTitleOffset(1.600, "Y")
	default_root_style.SetLabelOffset(0.010, "Y")
	default_root_style.SetLabelSize(0.040, "Y")
	default_root_style.SetLabelFont(42, "Y")

	default_root_style.SetTitleSize(0.055, "Z")
	default_root_style.SetTitleOffset(1.600, "Z")
	default_root_style.SetLabelOffset(0.010, "Z")
	default_root_style.SetLabelSize(0.040, "Z")
	default_root_style.SetLabelFont(42, "Z")

	default_root_style.SetTextSize(0.055)
	default_root_style.SetTextFont(42)

	default_root_style.SetStatFont(42)
	default_root_style.SetTitleFont(42)
	default_root_style.SetTitleFont(42, "X")
	default_root_style.SetTitleFont(42, "Y")

	default_root_style.SetOptStat(0)
	
	default_root_style.SetPalette(1)
	
	default_root_style.cd()
	ROOT.gStyle = default_root_style


def make_canvas(name, title, dx=600, dy=600):

	# Start with a canvas
	canvas = ROOT.TCanvas(name, title, 0, 0, dx, dy)
	
	# General overall stuff
	canvas.SetFillColor(0)
	canvas.SetBorderMode(0)
	canvas.SetBorderSize(10)
	
	# Set margins to reasonable defaults
	canvas.SetLeftMargin(0.18)
	canvas.SetRightMargin(0.05)
	canvas.SetTopMargin(0.08)
	canvas.SetBottomMargin(0.15)
	
	# Setup a frame which makes sense
	canvas.SetFrameFillStyle(0)
	canvas.SetFrameLineStyle(0)
	canvas.SetFrameBorderMode(0)
	canvas.SetFrameBorderSize(10)
	canvas.SetFrameFillStyle(0)
	canvas.SetFrameLineStyle(0)
	canvas.SetFrameBorderMode(0)
	canvas.SetFrameBorderSize(10)

	return canvas


def init_sub_pad(sub_pad):
	sub_pad.cd()
	sub_pad.SetLeftMargin(0.18)
	sub_pad.SetTopMargin(0.05)
	sub_pad.SetRightMargin(0.07)
	sub_pad.SetBottomMargin(0.15)


def init_signal(hist):
	hist.SetFillStyle(0.)
	hist.SetLineStyle(11)
	hist.SetLineWidth(3.)
	hist.SetLineColor(ROOT.kBlue)
	#hist.SetLineColor(ROOT.kBlue+3)


def init_hist(hist, x_title, y_title, color, style):
	hist.SetXTitle(x_title)
	hist.SetYTitle(y_title)
	hist.SetLineColor(ROOT.kBlack)
	hist.SetLineWidth(3.)
	hist.SetFillColor(color)
	hist.SetFillStyle(style)
	hist.SetTitleSize(0.055, "Y")
	hist.SetTitleOffset(1.600, "Y")
	hist.SetLabelOffset(0.014, "Y")
	hist.SetLabelSize(0.040, "Y")
	hist.SetLabelFont(42, "Y")
	hist.SetTitleSize(0.055, "X")
	hist.SetTitleOffset(1.300, "X")
	hist.SetLabelOffset(0.014, "X")
	hist.SetLabelSize(0.040, "X")
	hist.SetLabelFont(42, "X")
	hist.SetMarkerStyle(20)
	hist.SetMarkerColor(color)
	hist.SetMarkerSize(0.6)
	hist.GetYaxis().SetTitleFont(42)
	hist.GetXaxis().SetTitleFont(42)
	hist.SetTitle("")


def init_data(hist):
	hist.SetMarkerStyle(20.)
	hist.SetMarkerSize(1.3)
	hist.SetLineWidth(3.)


def set_legend_style(legend):
	legend.SetFillStyle(0)
	legend.SetFillColor(0)
	legend.SetBorderSize(0)
