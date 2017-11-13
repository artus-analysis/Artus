/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef TBPROGRESSMONITOR_H
#define TBPROGRESSMONITOR_H

#include <ostream>
#include <sys/time.h>

class ProgressMonitor
{
public:
	ProgressMonitor(const long long nPos = 0, const bool bInstant = false, const unsigned long updateInterval = 1000);
	~ProgressMonitor();
	bool Update();
	bool Update(const long long cPos);
	void IncMax(const long long nPos);
	void Reset();
private:
	bool bShow, bInstant;
	long long nPos, cPos;
	const unsigned long updateInterval;
	struct timeval tStartTime, tLastUpdate;
	static bool bAbort;
	static void CatchSignal(int);
	friend std::ostream &operator<<(std::ostream &os, ProgressMonitor &pm);
};

std::ostream &operator<<(std::ostream &os, ProgressMonitor &pm);

#endif
