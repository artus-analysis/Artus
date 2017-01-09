
#include "Artus/Consumer/interface/Profile2D.h"

#include "Artus/Utility/interface/RootFileHelper.h"


Profile2d::DataPoint::DataPoint(double x, double y, double weight) :
		m_fx(x), m_fy(y), m_fweight(weight) {
}

Profile2d::Profile2d(std::string sName, std::string sFolder) :
	ProfileBase<Profile2d>(sName, sFolder),
	m_iBinCountX(100),
	m_dBinLowerX(100),
	m_dBinUpperX(100)
{
}

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

