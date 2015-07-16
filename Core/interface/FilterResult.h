/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <list>
#include <vector>

#include "Artus/Core/interface/Cpp11Support.h"
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
		DecisionEntry() : filterName(""),
		                  filterDecision(Decision::Undefined),
		                  taggingMode(TaggingMode::Filtering) {}
		DecisionEntry(std::string filterName, Decision filterDecision, TaggingMode taggingMode) :
		              filterName(filterName), filterDecision(filterDecision), taggingMode(taggingMode) {}
	};

	typedef std::list<DecisionEntry> FilterDecisions;

	FilterResult();
	FilterResult(FilterNames const& initialFilterNames );
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
