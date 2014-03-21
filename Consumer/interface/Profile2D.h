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
		DataPoint(double x, double y, double weight) :
				m_fx(x), m_fy(y), m_fweight(weight) {

		}

		double m_fx, m_fy, m_fweight;
	};

	Profile2d(std::string sName, std::string sFolder) :
			ProfileBase<Profile2d>(sName, sFolder), m_iBinCountX(100), m_dBinLowerX(
					100), m_dBinUpperX(100) {
	}

	void Init();
	void Store(TFile * pRootFile);
	void AddPoint(double x, double y, double weight);

	unsigned int m_iBinCountX;
	double m_dBinLowerX;
	double m_dBinUpperX;

	std::vector<DataPoint> m_points;

	boost::scoped_ptr<TProfile> m_profile;
};

