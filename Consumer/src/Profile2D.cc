
#include "Artus/Consumer/interface/Profile2D.h"

#include "Artus/Utility/interface/RootFileHelper.h"


void Profile2d::Init() {
	RootFileHelper::SafeCd(gROOT, GetRootFileFolder());
	m_profile.reset(
			RootFileHelper::GetStandaloneTProfile(GetName().c_str(),
					GetName().c_str(), m_iBinCountX, m_dBinLowerX,
					m_dBinUpperX));
}

void Profile2d::Store(TFile * pRootFile) {
	RootFileHelper::SafeCd(pRootFile, GetRootFileFolder());
	m_profile->Write(GetName().c_str());
}

void Profile2d::AddPoint(double x, double y, double weight) {
	m_profile->Fill(x, y, weight);
}

