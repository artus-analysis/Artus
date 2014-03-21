
#include <algorithm>
#include <sstream>

#include "Artus/Core/interface/FilterResult.h"


FilterResult::FilterResult() :
		// has passed by default
		m_cacheHasPassed(true), m_IsCachedHasPassed(false) {
}

FilterResult::FilterResult(FilterNames const& initialFilterNames ) :
		// has passed by default
		m_cacheHasPassed(true), m_IsCachedHasPassed(false) {

	AddFilterNames ( initialFilterNames );
}

FilterResult::DecisionEntry * FilterResult::GetDecisionEntry( std::string const& filterName ) {
	for ( FilterResult::FilterDecisions::iterator it = m_filterDecisions.begin();
			it != m_filterDecisions.end(); it ++ ) {
		if ( filterName == it->first )
			return & ( *it );
	}

	return ARTUS_CPP11_NULLPTR;
}

FilterResult::DecisionEntry const* FilterResult::GetDecisionEntry( std::string const& filterName ) const {
	for ( FilterResult::FilterDecisions::const_iterator it = m_filterDecisions.begin();
			it != m_filterDecisions.end(); it ++ ) {
		if ( filterName == it->first )
			return & ( *it );
	}

	return ARTUS_CPP11_NULLPTR;
}

// add a list of filter names
// will only add and set the decision to undefined, if the
// name is not in the list before
void FilterResult::AddFilterNames( FilterNames const& fn ){
	for ( FilterResult::FilterNames::const_iterator it = fn.begin();
			it != fn.end(); it ++ ) {
		FilterResult::DecisionEntry * entr = GetDecisionEntry( * it );
		if ( entr == ARTUS_CPP11_NULLPTR ) {
			m_filterDecisions.push_back( std::make_pair( *it, Decision::Undefined ));
		}
	}
}

// list of all filter names as a vector of strings
FilterResult::FilterNames FilterResult::GetFilterNames() const {
	FilterNames filterNames;
	filterNames.resize(m_filterDecisions.size());
	transform(m_filterDecisions.begin(), m_filterDecisions.end(), filterNames.begin(),
	          [](FilterResult::DecisionEntry decisionEntry) { return decisionEntry.first; });
	return filterNames;
}

bool FilterResult::HasPassed() const {
	if (m_IsCachedHasPassed)
		return m_cacheHasPassed;

	m_cacheHasPassed = true;
	for (FilterResult::FilterDecisions::const_iterator it = GetFilterDecisions().begin();
			it != GetFilterDecisions().end(); it++) {
		if (it->second == FilterResult::Decision::NotPassed)
			m_cacheHasPassed = false;
	}

	m_IsCachedHasPassed = true;
	return m_cacheHasPassed;
}

bool FilterResult::HasPassedIfExcludingFilter(std::string const& excludedFilter) const {
	for (FilterResult::FilterDecisions::const_iterator it = GetFilterDecisions().begin();
			it != GetFilterDecisions().end(); it++) {
		if (it->second == FilterResult::Decision::NotPassed)
			if (it->first != excludedFilter)
				return false;
	}

	return true;
}

FilterResult::Decision FilterResult::GetFilterDecision(std::string filterName) const {
	FilterResult::DecisionEntry const* entr = GetDecisionEntry(filterName);

	if ( entr == ARTUS_CPP11_NULLPTR ){
		LOG_FATAL( "Decision entry with name " + filterName + " not found" );
	}
	return entr->second;
}

FilterResult::FilterDecisions const& FilterResult::GetFilterDecisions() const {
	return m_filterDecisions;
}

void FilterResult::SetFilterDecision(std::string filterName, bool passed) {
	FilterResult::Decision desc = passed ? FilterResult::Decision::Passed : FilterResult::Decision::NotPassed;
	FilterResult::DecisionEntry * entr = GetDecisionEntry(filterName);

	if ( entr == ARTUS_CPP11_NULLPTR ) {
		m_filterDecisions.push_back( std::make_pair( filterName, desc ));
	} else {
		entr->second = desc;
	}
	
	// recompute the cached filter result, if the new filter
	// decision might change this
	if (m_IsCachedHasPassed && ( m_cacheHasPassed != passed ))
		m_IsCachedHasPassed = false;
}

std::string FilterResult::ToString() const {
	std::stringstream s;
	s << "== Filter Decision == " << std::endl;

	for (FilterResult::FilterDecisions::const_iterator it = m_filterDecisions.begin();
			it != m_filterDecisions.end(); it++) {
		s << it->first << " : " << FilterResult::DecisionToString( it->second ) << std::endl;
	}

	return s.str();
}

std::string FilterResult::DecisionToString ( Decision dc ) const {
	if ( dc == Decision::Passed )
		return "passed";
	else if ( dc == Decision::NotPassed )
		return "not passed";
	else
		return "undefined";
}
