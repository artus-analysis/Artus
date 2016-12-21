
#pragma once

#include <list>

#include <boost/noncopyable.hpp>

#include "FilterResult.h"

class CutFlow: public boost::noncopyable
{
public:

	typedef std::pair<std::string, long> CutStat;
	typedef std::list<CutStat> CutCount;

	CutFlow();

	// sum up all passed events per filter
	void AddFilterResult(FilterResult const& fres);

	CutStat * GetCutEntry(std::string const& filterName);

	CutCount const& GetCutCount() const;

	long GetEventCount() const;

	std::string ToString() const;

private:
	CutCount m_cutCount;
	long m_overallEventCount;
};
