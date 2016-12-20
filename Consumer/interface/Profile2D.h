/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/scoped_ptr.hpp>

#include "TROOT.h"
#include "TProfile.h"
#include "TFile.h"

#include "ProfileBase.h"

class Profile2d: public ProfileBase<Profile2d> {
public:

	class DataPoint {
	public:
		DataPoint(double x, double y, double weight);
		double m_fx, m_fy, m_fweight;
	};

	Profile2d(std::string sName, std::string sFolder);

	void Init();
	void Store(TFile * pRootFile);
	void AddPoint(double x, double y, double weight);

	unsigned int m_iBinCountX;
	double m_dBinLowerX;
	double m_dBinUpperX;

	std::vector<DataPoint> m_points;

	boost::scoped_ptr<TProfile> m_profile;
};

