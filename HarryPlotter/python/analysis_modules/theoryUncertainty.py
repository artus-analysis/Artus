# -*- coding: utf-8 -*-


import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import hashlib
import math

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


def rms(ivec): return math.sqrt(sum(i*i for i in ivec) / len(ivec))

def mse(ivec, central = 0):
	if central == 0:
		return rms([ (value - rms(ivec)) for value in ivec])
	else:
		return rms([ (value - central) for value in ivec])

class TheoryUncertainty(analysisbase.AnalysisBase):
	"""Calculating theoretical uncertainties via acceptance + control plots"""

	def __init__(self):
		super(TheoryUncertainty, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(TheoryUncertainty, self).modify_argument_parser(parser, args)
		
		self.efficiency_options = parser.add_argument_group("Acceptance options")
		self.efficiency_options.add_argument("--theoryuncertainty-numerator", nargs="+", default=[None], help="Nick names for the numerators of the acceptance.")
		self.efficiency_options.add_argument("--theoryuncertainty-denominator", nargs="+", default=[None], help="Nick names for the denominator of the acceptance.")
		self.efficiency_options.add_argument("--theoryuncertainty-centralvalue", nargs="+", default=[None], help="Nick names for the central value.")
		#-r

	def prepare_args(self, parser, plotData):
		super(TheoryUncertainty, self).prepare_args(parser, plotData)
		
		plotData.plotdict["theoryuncertainty_numerator"] = [nick for nick in plotData.plotdict["theoryuncertainty_numerator"] if nick in plotData.plotdict["root_objects"]]
		plotData.plotdict["theoryuncertainty_denominator"] = [nick for nick in plotData.plotdict["theoryuncertainty_denominator"] if nick in plotData.plotdict["root_objects"]]
		self.prepare_list_args(plotData, ["theoryuncertainty_numerator", "theoryuncertainty_denominator"])
		
	def run(self, plotData=None):
		super(TheoryUncertainty, self).run(plotData)
		A = {}
		central_value = ""
		for index, (theoryuncertainty_numerator, theoryuncertainty_denominator ) in enumerate(zip(plotData.plotdict["theoryuncertainty_numerator"], plotData.plotdict["theoryuncertainty_denominator"])):
			if plotData.plotdict["root_objects"][theoryuncertainty_denominator].Integral() != 0 :
				A[theoryuncertainty_numerator] = plotData.plotdict["root_objects"][theoryuncertainty_numerator].Integral() / plotData.plotdict["root_objects"][theoryuncertainty_denominator].Integral() 
			else:
				print plotData.plotdict["filename"], "DIV 0", theoryuncertainty_numerator
				A[theoryuncertainty_numerator] = float('nan')
			if True or log.isEnabledFor(logging.DEBUG):  
				print theoryuncertainty_numerator, ":", plotData.plotdict["root_objects"][theoryuncertainty_numerator].Integral(), "/", plotData.plotdict["root_objects"][theoryuncertainty_denominator].Integral() , "=", A[theoryuncertainty_numerator]
			#plotData.plotdict["root_objects"][theoryuncertainty_numerator].Print()
		
		miniA = next((x for x in sorted(A.values()) if x), 0)
		unc = -1
		if A[plotData.plotdict["theoryuncertainty_centralvalue"]] == 0:
			print plotData.plotdict["filename"], "A[plotData.plotdict[theoryuncertainty_centralvalue]] = 0", 'where theoryuncertainty_centralvalue: ',  plotData.plotdict["theoryuncertainty_centralvalue"]
		else:
			print "WRONG" # higgsplot.py -m line -j DY1050json/merged_ztt_et_scale_only_2bjet.json

			if "alphas" in plotData.plotdict["filename"]:#REDO it to not read the filename from the json but just take the filename(unexpected turn of events)
				alpha1 = 0.118
				alpha2 = 0.130
				alpha = alpha2 + 0.0015
				if log.isEnabledFor(logging.DEBUG): print "for the alpha =",alpha
				Anew = []
				deltaAold = 0 # A_118 - A_130
				for i, Aval in enumerate(A.values()):
					if i == 1: # corresponds to 0.130
						deltaAold -= Aval
						Anew.append(Aval)
					else:# corresponds to 0.118
						deltaAold += Aval
				Anew.append( ((alpha - alpha2) / (alpha1 - alpha2) * deltaAold) + Anew[0])		
				unc = (max(Anew) - min(Anew)) / A[plotData.plotdict["theoryuncertainty_centralvalue"]]
				if True or log.isEnabledFor(logging.DEBUG): 
					print "For alphas_unc: (", max(Anew), "-", min(Anew), ") / ", A[plotData.plotdict["theoryuncertainty_centralvalue"]], " = ", unc
			elif "pdf" in plotData.plotdict["filename"]:
				unc = mse(A.values(), A[plotData.plotdict["theoryuncertainty_centralvalue"]]) / A[plotData.plotdict["theoryuncertainty_centralvalue"]]
			else:
				unc = (max(A.values()) - miniA) / 2. / A[plotData.plotdict["theoryuncertainty_centralvalue"]]
			if True or log.isEnabledFor(logging.DEBUG): print plotData.plotdict["filename"], "For_unc: (", max(A.values()), "-", miniA, ") / 2 / ", A[plotData.plotdict["theoryuncertainty_centralvalue"]], "=", (max(A.values()) - miniA) / 2. / A[plotData.plotdict["theoryuncertainty_centralvalue"]]
		if min(A.values()) < 0: print plotData.plotdict["filename"], "MIN VAL WAS < 0"
		print plotData.plotdict["filename"], "Unc:", unc 
		exit(1)