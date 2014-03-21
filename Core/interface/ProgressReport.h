/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Cpp11Support.h"

class ProgressReportBase {
public:
	virtual ~ProgressReportBase() {  };

	virtual void update(long long currentIndex, long long maxItems) = 0;

	virtual void finish(long long currentIndex, long long maxItems) = 0;
};

class ConsoleProgressReport: public ProgressReportBase {
public:

	ConsoleProgressReport(float reportIntervall = 0.1f);
	
	virtual ~ConsoleProgressReport() {  };

	virtual void update(long long currentIndex, long long maxItems) ARTUS_CPP11_OVERRIDE;

	virtual void finish(long long currentIndex, long long maxItems) ARTUS_CPP11_OVERRIDE;

	// in percent
	float m_reportIntervall;
	float m_lastReport;
};
