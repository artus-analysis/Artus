
#pragma once

#include <list>
#include <vector>

#include "Artus/Utility/interface/ArtusLogging.h"

class FilterResult {
public:

	enum class Decision { Undefined, Passed, NotPassed };
	enum class TaggingMode { Tagging, Filtering };

	typedef std::vector <std::string> FilterNames;
	//typedef std::map<std::string, Decision> FilterDecisions;
	struct DecisionEntry
	{
		std::string filterName;
		Decision filterDecision;
		TaggingMode taggingMode;
		DecisionEntry() ;
		DecisionEntry(std::string filterName, Decision filterDecision, TaggingMode taggingMode);
	};

	typedef std::list<DecisionEntry> FilterDecisions;

	FilterResult();
	explicit FilterResult(FilterNames const& initialFilterNames);
	FilterResult(FilterNames const& initialFilterNames, FilterNames const& taggingFilters );

	DecisionEntry * GetDecisionEntry( std::string const& filterName );
	DecisionEntry const* GetDecisionEntry( std::string const& filterName ) const;

	// add a list of filter names
	// will only add and set the decision to undefined, if the
	// name is not in the list before
	void AddFilterNames( FilterNames const& fn);
	void AddFilterNames( FilterNames const& fn, FilterNames const& taggingFilters);
	
	// list of all filter names as a vector of strings
	FilterNames GetFilterNames() const;
	
	bool HasPassed() const;
	bool HasPassedIfExcludingFilter(std::string const& excludedFilter) const;
	TaggingMode IsTaggingFilter(std::string const& filterName) const;
	Decision GetFilterDecision(std::string filterName) const;
	FilterDecisions const& GetFilterDecisions() const;
	void SetFilterDecision(std::string filterName, bool passed);
	std::string ToString() const;
	std::string DecisionToString ( Decision dc ) const;

private:

	// optimize this, without strings
	FilterNames m_taggingFilters;
	FilterDecisions m_filterDecisions;

	mutable bool m_cacheHasPassed;
	mutable bool m_IsCachedHasPassed;
};
