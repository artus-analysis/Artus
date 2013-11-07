/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <TH1D.h>
#include <TFile.h>

#include "HistBase.h"
#include "ValueModifierFwd.h"

/*
	Wrapper for Root's TH1D histograms.
	Enables flexible binning and range setup and takes 
	care of storing the TH1D into the proper ROOT folder.
*/

class Hist1D: public HistBase<Hist1D> {
public:

	Hist1D(std::string sName, std::string sFolder, ValueModifiers l);

	void Init();

	void Store(TFile* pRootFile);

	void Fill(double val, double weight);

	int m_iBinCount;
	double m_dBinLower;
	double m_dBinUpper;
	double m_dCustomBins[255];
	bool m_bUseCustomBin;

	ValueModifiers m_modifiers;
	boost::scoped_ptr<TH1D> m_hist;
};

