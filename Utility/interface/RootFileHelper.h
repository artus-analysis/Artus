#pragma once

#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TDirectory.h>
#include <TGraphErrors.h>


class RootFileHelper {
public:

	template<class T>
	static T SafeGet(TDirectory * pDir, std::string const& objName) {
		T ob = (T) pDir->Get(objName.c_str());

		if (ob == NULL) {
			std::cout << std::endl << "Cant load " << objName
					<< " from directory " << pDir->GetName() << std::endl;
			exit(0);
		}
		return ob;
	}

	static void SafeCd(TDirectory * pDir, std::string const& dirName) {
		assert(pDir);

		if (pDir->GetDirectory(dirName.c_str()) == 0) {
			pDir->mkdir(dirName.c_str());
		}
		pDir->cd(dirName.c_str());
	}
	static TH1D * GetStandaloneTH1D_1(std::string sName, std::string sCaption,
			int binCount, double dCustomBins[255]) {
		return new TH1D(sName.c_str(), sCaption.c_str(), binCount,
				&dCustomBins[0]);
	}

	static TH1D * GetStandaloneTH1D_2(std::string sName, std::string sCaption,
			int binCount, double min, double max) {
		return new TH1D(sName.c_str(), sCaption.c_str(), binCount, min, max);
	}

	static TProfile * GetStandaloneTProfile(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper) {
		return new TProfile(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
				m_dBinXLower, m_dBinXUpper);
	}

	static TGraphErrors * GetStandaloneTGraphErrors(int i) {
		return new TGraphErrors(i);
	}

	static TH2D * GetStandaloneTH2D_1(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper) {
		return new TH2D(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
				m_dBinXLower, m_dBinXUpper, m_iBinYCount, m_dBinYLower,
				m_dBinYUpper);
	}

	static TProfile2D * GetStandaloneTProfile2D(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper) {
		return new TProfile2D(m_sName.c_str(), m_sCaption.c_str(), m_iBinXCount,
				m_dBinXLower, m_dBinXUpper, m_iBinYCount, m_dBinYLower,
				m_dBinYUpper);
	}

};
