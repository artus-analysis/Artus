/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

#include "FilterResult.h"

class CutFlow: public boost::noncopyable
{
public:

	typedef std::pair<std::string, long> CutStat;
	typedef std::list<CutStat> CutCount;

	CutFlow() :
			m_overallEventCount(0)
	{

	}

	// sum up all passed events per filter
	void AddFilterResult(FilterResult const& fres);

	CutStat * GetCutEntry(std::string const& filterName);

	CutCount const& GetCutCount() const
	{
		return m_cutCount;
	}

	long GetEventCount() const
	{
		return m_overallEventCount;
	}

	std::string ToString() const;

private:
	CutCount m_cutCount;
	long m_overallEventCount;
};
