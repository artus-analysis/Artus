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

	// sum up all passed events per filter
	void AddFilterResult(FilterResult const& fres)
	{
		m_overallEventCount++;

		auto const& dec = fres.GetFilterDecisions();
		for (FilterResult::FilterDecisions::const_iterator it = dec.begin();
				it != dec.end(); it++)
		{
			// only store, if passed
			long addVal = 0;
			if (it->second == FilterResult::Decision::Passed) {
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
		sOut << "Cut Name \t\t | Events passed \t\t| Percentage" << std::endl;
		sOut << "===========================================================================" << std::endl;

		sOut << " -> Events before cuts : " << m_overallEventCount;
		sOut << std::endl;

		for (CutCount::const_iterator it = m_cutCount.begin();
				it != m_cutCount.end(); it++)
		{
			const float ratioPassed = float( it->second ) / float( m_overallEventCount );

			sOut << it->first << "\t\t";
			sOut << "| " << it->second << "\t\t\t\t";
			sOut << "| " << std::setprecision(2) << ( ratioPassed * 100.0f );

			sOut << std::endl;
		}

		return sOut.str();
	}

private:
	CutCount m_cutCount;
	long m_overallEventCount;
};
