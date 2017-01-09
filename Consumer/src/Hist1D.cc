
#include "Artus/Consumer/interface/Hist1D.h"

#include "TROOT.h"

#include "Artus/Consumer/interface/ValueModifier.h"
#include "Artus/Utility/interface/RootFileHelper.h"

Hist1D::Hist1D(std::string sName, std::string sFolder, ValueModifiers l) :
		HistBase<Hist1D>(sName, sFolder), m_iBinCount(100), m_dBinLower(0.0f), m_dBinUpper(
				200.0f), m_bUseCustomBin(false), m_modifiers(l), m_hist(nullptr) {

	// apply modifiers
	for (auto const& m : m_modifiers) {
		m->applyHistBeforeCreation(this, 0);
	}

	RootFileHelper::SafeCd(gROOT, GetRootFileFolder());

	// create with custom TH1D binning, or not ...
	if (m_bUseCustomBin) {
		m_hist.reset(
				RootFileHelper::GetStandaloneTH1D_1(GetName(), GetCaption(),
						m_iBinCount, &m_dCustomBins[0]));
	} else {
		m_hist.reset(
				RootFileHelper::GetStandaloneTH1D_2(
						GetRootFileFolder() + "_" + GetName(), GetCaption(),
						this->m_iBinCount, this->m_dBinLower,
						this->m_dBinUpper));
	}
	// store sum of weights^2 per bin to take the weight into consideration
	// for the error computation
	// From ROOT docu:
	//  If Sumw2 has been called, the error per bin is computed as
	//  the sqrt(sum of squares of weights), otherwise the error is
	//  set equal to the sqrt(bin content).
	m_hist->Sumw2();
}

void Hist1D::Init() {

}

void Hist1D::Store(TFile* pRootFile) {
	assert(pRootFile);
	LOG(INFO) << "Storing Histogram " << this->GetRootFileFolder() << "/" << this->GetName() << ".";

	RootFileHelper::SafeCd(pRootFile, GetRootFileFolder());
	m_hist->Write((this->GetName()).c_str());
}

void Hist1D::Fill(double val, double weight) {
	m_hist->Fill(val, weight);
}

