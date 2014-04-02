/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TDirectory.h>
#include <TGraphErrors.h>

#include "Artus/Core/interface/GlobalInclude.h"

class RootFileHelper {
public:

	template<class T>
	static T SafeGet(TDirectory * pDir, std::string const& objName) {
		T ob = (T) pDir->Get(objName.c_str());

		if (ob == NULL) {
			LOG(FATAL) << "Cant load " << objName << " from directory " << pDir->GetName() << "!";
		}
		return ob;
	}

	static void SafeCd(TDirectory * pDir, std::string const& dirName);
	static TH1D * GetStandaloneTH1D_1(std::string sName, std::string sCaption,
			int binCount, double dCustomBins[255]);
	static TH1D * GetStandaloneTH1D_2(std::string sName, std::string sCaption,
			int binCount, double min, double max);
	static TProfile * GetStandaloneTProfile(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper);
	static TGraphErrors * GetStandaloneTGraphErrors(int i);
	static TH2D * GetStandaloneTH2D_1(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper);
	static TProfile2D * GetStandaloneTProfile2D(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper);

};
