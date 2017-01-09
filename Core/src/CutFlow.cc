#include <sstream>
#include <map>
#include <iomanip>

#include "Artus/Core/interface/CutFlow.h"


CutFlow::CutFlow() :
		m_overallEventCount(0)
{
}

// sum up all passed events per filter
void CutFlow::AddFilterResult(FilterResult const& fres)
{
	++m_overallEventCount;

	auto const& dec = fres.GetFilterDecisions();
	for (FilterResult::FilterDecisions::const_iterator it = dec.begin();
	     it != dec.end(); ++it)
	{
		// only store, if passed
		long addVal = 0;
		if (it->filterDecision == FilterResult::Decision::Passed &&
		    it->taggingMode == FilterResult::TaggingMode::Filtering) {
			addVal = 1;
		}

		CutFlow::CutStat * stat = CutFlow::GetCutEntry(it->filterName);
		if (stat == nullptr)
		{
			m_cutCount.push_back(std::make_pair(it->filterName, addVal));
		}
		else
		{
			stat->second += addVal;
		}
	}
}

CutFlow::CutStat * CutFlow::GetCutEntry(std::string const& filterName)
{
	for (CutFlow::CutCount::iterator it = m_cutCount.begin();
	     it != m_cutCount.end(); ++it)
	{
		if (filterName == it->first)
			return &(*it);
	}

	return nullptr;
}

CutFlow::CutCount const& CutFlow::GetCutCount() const
{
	return m_cutCount;
}

long CutFlow::GetEventCount() const
{
	return m_overallEventCount;
}

std::string CutFlow::ToString() const
{
	std::stringstream sOut;

	sOut << std::endl;
	sOut << "Cut Name \t\t | Events passed \t\t| Percentage" << std::endl;
	sOut << "===========================================================================" << std::endl;

	sOut << " -> Events before cuts : " << m_overallEventCount;
	sOut << std::endl;

	for (CutFlow::CutCount::const_iterator it = m_cutCount.begin();
	     it != m_cutCount.end(); ++it)
	{
		const float ratioPassed = static_cast<float>(it->second) / static_cast<float>(m_overallEventCount);

		sOut << it->first << "\t\t";
		sOut << "| " << it->second << "\t\t\t\t";
		sOut << "| " << std::setprecision(2) << ( ratioPassed * 100.0f );

		sOut << std::endl;
	}

	return sOut.str();
}
