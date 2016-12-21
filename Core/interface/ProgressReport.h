
#pragma once

class ProgressReportBase {
public:
	virtual ~ProgressReportBase();

	virtual void update(long long currentIndex, long long maxItems) = 0;

	virtual void finish() = 0;
};

class ConsoleProgressReport: public ProgressReportBase {
public:

	explicit ConsoleProgressReport(float reportIntervall = 0.1f);

	virtual ~ConsoleProgressReport();

	void update(long long currentIndex, long long maxItems) override;

	void finish() override;

	// in percent
	float m_reportIntervall;
	float m_lastReport;
};
