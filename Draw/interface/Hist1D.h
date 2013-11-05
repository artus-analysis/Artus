#pragma once

#include <TH1D.h>
#include <TFile.h>

#include "HistBase.h"
#include "ValueModifierFwd.h"

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

