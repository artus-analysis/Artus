/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include <iostream>
#include <iomanip>
#include <signal.h>
#include "Artus/KappaTools/interface/ProgressMonitor.h"
#include "Artus/KappaTools/interface/IOHelper.h"

using namespace std;

bool ProgressMonitor::bAbort = false;

void ProgressMonitor::CatchSignal(int sig)
{
	bAbort = true;
}

ProgressMonitor::ProgressMonitor(const long long nPos, const bool bInstant, const unsigned long _updateInterval)
	: updateInterval(_updateInterval)
{
	bShow = isatty(1);
	signal(SIGINT, CatchSignal);
	this->nPos = nPos;
	this->bInstant = bInstant && bShow;
	Reset();
}

ProgressMonitor::~ProgressMonitor()
{
	signal(SIGINT, SIG_DFL);
	if (bShow)
		cout << "\r";
	cout << "Status: " << *this << endl;
}

bool ProgressMonitor::Update()
{
	return Update(++(this->cPos));
}

bool ProgressMonitor::Update(const long long cPos)
{
	this->cPos = cPos;
	if (bShow && (cPos % updateInterval == 0))
	{
		cout << "\rStatus: ";
		cout << *this;
		cout.flush();
	}
	if (!bShow && (cPos % 50000 == 0))
		cout << *this << endl;
	return !bAbort;
}

void ProgressMonitor::IncMax(const long long nPos)
{
	this->nPos += nPos;
	Reset();
}

void ProgressMonitor::Reset()
{
	cPos = 0;
	gettimeofday(&tStartTime, 0);
	gettimeofday(&tLastUpdate, 0);
}

struct SecTime
{
	SecTime(const long _sec) : sec(_sec) {}
	long sec;
};

ostream &operator<<(ostream &os, const SecTime &st)
{
	return os
		<< setw(2) << setfill('0') << st.sec / 3600 << ":"
		<< setw(2) << setfill('0') << (st.sec / 60) % 60 << ":"
		<< setw(2) << setfill('0') << st.sec % 60;
}

ostream &operator<<(ostream &os, ProgressMonitor &pm)
{
	OStreamGuard guard(os);
	struct timeval now;
	gettimeofday(&now, 0);
	const double realSpeed = pm.cPos / (double)(now.tv_sec - pm.tStartTime.tv_sec + 1.0e-6 * (now.tv_usec - pm.tStartTime.tv_usec));
	const double calcSpeed = (realSpeed < 1) ? 1 : realSpeed;
	const unsigned long sec = (unsigned long)((pm.nPos - pm.cPos) / calcSpeed);
	os << pm.cPos << "/" << pm.nPos << " - "
		<< SecTime(sec) << "/" << SecTime((int)(pm.nPos / calcSpeed)) << " - "
		<< (int)realSpeed << " / s" << " - ";
	if (pm.bInstant)
	{
		const double curSpeed = pm.updateInterval / (double)(now.tv_sec - pm.tLastUpdate.tv_sec + 1.0e-6 * (now.tv_usec - pm.tLastUpdate.tv_usec));
		gettimeofday(&(pm.tLastUpdate), 0);
		os << (int)curSpeed << " / s" << " - ";
	}
	if (pm.cPos < (unsigned int)(0.99 * pm.nPos))
		os << setw(3) << setfill(' ') << setprecision(2) << 100.0 * pm.cPos / pm.nPos << "%    ";
	else
		os << "100%    ";
	return os;
}
