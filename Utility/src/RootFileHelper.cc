
#include "Artus/Utility/interface/RootFileHelper.h"

#include <cassert>


void RootFileHelper::SafeCd(TDirectory * pDir, std::string const& dirName) {
	assert(pDir);

	if (pDir->GetDirectory(dirName.c_str()) == nullptr) {
		pDir->mkdir(dirName.c_str());
	}
	pDir->cd(dirName.c_str());
}
TH1D * RootFileHelper::GetStandaloneTH1D_1(std::string sName, std::string sCaption,
		int binCount, double dCustomBins[255]) {
	return new TH1D(sName.c_str(), sCaption.c_str(), binCount,
			&dCustomBins[0]);
}

TH1D * RootFileHelper::GetStandaloneTH1D_2(std::string sName, std::string sCaption,
		int binCount, double min, double max) {
	return new TH1D(sName.c_str(), sCaption.c_str(), binCount, min, max);
}

TProfile * RootFileHelper::GetStandaloneTProfile(std::string m_sName,
		std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
		double m_dBinXUpper) {
	return new TProfile(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
			m_dBinXLower, m_dBinXUpper);
}

TGraphErrors * RootFileHelper::GetStandaloneTGraphErrors(int i) {
	return new TGraphErrors(i);
}

TH2D * RootFileHelper::GetStandaloneTH2D_1(std::string m_sName,
		std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
		double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
		double m_dBinYUpper) {
	return new TH2D(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
			m_dBinXLower, m_dBinXUpper, m_iBinYCount, m_dBinYLower,
			m_dBinYUpper);
}

TProfile2D * RootFileHelper::GetStandaloneTProfile2D(std::string m_sName,
		std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
		double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
		double m_dBinYUpper) {
	return new TProfile2D(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
			m_dBinXLower, m_dBinXUpper, m_iBinYCount, m_dBinYLower,
			m_dBinYUpper);
}
