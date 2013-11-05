#include "Artus/Draw/interface/Hist1D.h"

#include "TROOT.h"

#include "Artus/Draw/interface/ValueModifier.h"

#include "Artus/Utility/interface/RootFileHelper.h"

Hist1D::Hist1D(std::string sName, std::string sFolder, ValueModifiers l) :
		HistBase<Hist1D>(sName, sFolder), m_iBinCount(100), m_dBinLower(0.0f), m_dBinUpper(
				200.0f), m_bUseCustomBin(false), m_modifiers(l) {

	// apply modifiers
	for (auto const& m : m_modifiers) {
		m->applyHistBeforeCreation(this, 0);
	}

	RootFileHelper::SafeCd(gROOT, GetRootFileFolder());
	if (m_bUseCustomBin) {
		m_hist.reset(
				RootFileHelper::GetStandaloneTH1D_1(GetName(), GetCaption(),
						m_iBinCount, &m_dCustomBins[0]));
		/*new TH1D(this->m_sName.c_str(),
		 this->m_sCaption.c_str(),
		 m_iBinCount, &m_dCustomBins[0] );*/
	} else {
		m_hist.reset(
				RootFileHelper::GetStandaloneTH1D_2(
						GetRootFileFolder() + "_" + GetName(), GetCaption(),
						this->m_iBinCount, this->m_dBinLower,
						this->m_dBinUpper));
	}
	m_hist->Sumw2();
}

void Hist1D::Store(TFile* pRootFile) {
	CALIB_LOG(
			"Storing Histogram " + this->GetRootFileFolder() + "/"
					+ this->GetName())

	RootFileHelper::SafeCd(pRootFile, GetRootFileFolder());
	m_hist->Write((this->GetName()).c_str());
}

void Hist1D::Fill(double val, double weight) {
	m_hist->Fill(val, weight);
}

