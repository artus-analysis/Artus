
#include <iostream>

#include "Artus/Core/interface/ProgressReport.h"


ProgressReportBase::~ProgressReportBase()
{
}

ConsoleProgressReport::~ConsoleProgressReport()
{
}

ConsoleProgressReport::ConsoleProgressReport(float reportIntervall) :
		m_reportIntervall(reportIntervall),
		m_lastReport(0.0f)
{
}

void ConsoleProgressReport::update(long long currentIndex, long long maxItems)
{
	const float ratio = static_cast<float>(currentIndex) / static_cast<float>(maxItems - 1);

	if ((ratio - m_reportIntervall > m_lastReport) ||
	// be always sure to report we reached the last item
			(currentIndex + 1 == maxItems)) {

		m_lastReport = ratio;
		std::cout << "\r" << " -- Events : " << currentIndex << " / "
		          << maxItems << " - " << (ratio * 100.0f) << " %                                          ";
		std::cout.flush();
	}
}

void ConsoleProgressReport::finish()
{
	std::cout << std::endl;
}
