/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Cpp11Support.h"
#include <iostream>

class ProgressReportBase {
public:
	virtual ~ProgressReportBase() {
	}

	virtual void update(long long currentIndex, long long maxItems) = 0;

	virtual void finish(long long currentIndex, long long maxItems) = 0;
};

class ConsoleProgressReport: public ProgressReportBase {
public:

	ConsoleProgressReport(float reportIntervall = 0.1f) :
			m_reportIntervall(reportIntervall), m_lastReport(0.0f) {
	}

	virtual ~ConsoleProgressReport() {
	}

	virtual void update(long long currentIndex, long long maxItems)
			ARTUS_CPP11_OVERRIDE
			{
		const float ratio = (float(currentIndex) / float(maxItems - 1));

		if (((ratio - m_reportIntervall) > m_lastReport) ||
		// be always sure to report we reached the last item
				((currentIndex + 1) == (maxItems))) {

			m_lastReport = ratio;
			std::cout << "\r" << " -- Events : " << currentIndex << " / "
					<< maxItems << " - " << (ratio * 100.0f) << " %                                          ";
			std::cout.flush();
		}
	}

	virtual void finish(long long currentIndex, long long maxItems)
			ARTUS_CPP11_OVERRIDE
			{
		std::cout << std::endl;
	}

	// in percent
	float m_reportIntervall;
	float m_lastReport;
};
