/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <sstream>
#include <map>
#include <iomanip>

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

	// this code assumes, that only one filter in the whole
	// has the result false, all other filter have not been rated
	// and filter result is in the correct order as the filters were
	// applied
	void AddFilterResult(FilterResult const& fres)
	{
		m_overallEventCount++;

		auto const& dec = fres.GetFilterDecisions();
		for (FilterResult::FilterDecisions::const_iterator it = dec.begin();
				it != dec.end(); it++)
		{
			// only store, if failed
			long addVal = 0;
			if (it->second == FilterResult::Decision::NotPassed) {
				addVal = 1;
			}

			CutStat * stat = GetCutEntry(it->first);
			if (stat == ARTUS_CPP11_NULLPTR)
			{
				m_cutCount.push_back(std::make_pair(it->first, addVal));
			}
			else
			{
				stat->second += addVal;
			}
		}
	}

	CutStat * GetCutEntry(std::string const& filterName)
	{
		for (CutCount::iterator it = m_cutCount.begin();
				it != m_cutCount.end(); it++)
		{
			if (filterName == it->first)
				return &(*it);
		}

		return ARTUS_CPP11_NULLPTR;
	}

	CutCount const& GetCutCount() const
	{
		return m_cutCount;
	}

	long GetEventCount() const
	{
		return m_overallEventCount;
	}

	std::string ToString() const
	{
		std::stringstream sOut;

		sOut << std::endl;
		sOut << "Cut Name \t\t | Events dropped \t\t| Percentage" << std::endl;
		sOut << "===========================================================================" << std::endl;

		sOut << " -> Events before cuts : " << m_overallEventCount;
		sOut << std::endl;

		long evtsDropped = 0;

		for (CutCount::const_iterator it = m_cutCount.begin();
				it != m_cutCount.end(); it++)
		{
			const float ratioDropped = float( it->second ) / float( m_overallEventCount );
			evtsDropped += it->second;

			sOut << it->first << "\t\t";
			sOut << "| " << it->second << "\t\t\t\t";
			sOut << "| " << std::setprecision(2) << ( ratioDropped * 100.0f );

			sOut << std::endl;
		}

		sOut << " -> Events passed : " << ( m_overallEventCount - evtsDropped) ;
		sOut << std::endl;

		return sOut.str();
	}

private:
	CutCount m_cutCount;
	long m_overallEventCount;
};
