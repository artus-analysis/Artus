/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalInclude.h"

class FilterResult {
public:

	enum class Decision { Undefined, Passed, NotPassed };

	typedef std::vector < std::string> FilterNames;
	//typedef std::map<std::string, Decision> FilterDecisions;

	typedef std::pair <std::string, Decision> DecisionEntry;
	typedef std::list<DecisionEntry> FilterDecisions;



	FilterResult() :
			// has passed by default
			m_cacheHasPassed(true), m_IsCachedHasPassed(false) {
	}

	FilterResult(FilterNames const& initialFilterNames ) :
			// has passed by default
			m_cacheHasPassed(true), m_IsCachedHasPassed(false) {

		AddFilterNames ( initialFilterNames );
	}

	DecisionEntry * GetDecisionEntry( std::string const& filterName ) {
		for ( FilterDecisions::iterator it = m_filterDecisions.begin();
				it != m_filterDecisions.end(); it ++ ) {
			if ( filterName == it->first )
				return & ( *it );
		}

		return ARTUS_CPP11_NULLPTR;
	}

	DecisionEntry const* GetDecisionEntry( std::string const& filterName ) const {
		for ( FilterDecisions::const_iterator it = m_filterDecisions.begin();
				it != m_filterDecisions.end(); it ++ ) {
			if ( filterName == it->first )
				return & ( *it );
		}

		return ARTUS_CPP11_NULLPTR;
	}

	// add a list of filter names
	// will only add and set the decision to undefined, if the
	// name is not in the list before
	void AddFilterNames( FilterNames const& fn ){
		for ( FilterNames::const_iterator it = fn.begin();
				it != fn.end(); it ++ ) {
			DecisionEntry * entr = GetDecisionEntry( * it );
			if ( entr == ARTUS_CPP11_NULLPTR ) {
				m_filterDecisions.push_back( std::make_pair( *it, Decision::Undefined ));
			}
		}
	}

	bool HasPassed() const {
		if (m_IsCachedHasPassed)
			return m_cacheHasPassed;

		m_cacheHasPassed = true;
		for (FilterDecisions::const_iterator it = GetFilterDecisions().begin();
				it != GetFilterDecisions().end(); it++) {
			if (it->second == Decision::NotPassed)
				m_cacheHasPassed = false;
		}

		m_IsCachedHasPassed = true;
		return m_cacheHasPassed;
	}

	bool HasPassedIfExcludingFilter(std::string const& excludedFilter) const {
		for (FilterDecisions::const_iterator it = GetFilterDecisions().begin();
				it != GetFilterDecisions().end(); it++) {
			if (it->second == Decision::NotPassed)
				if (it->first != excludedFilter)
					return false;
		}

		return true;
	}

	Decision GetFilterDecision(std::string filterName) const {
		DecisionEntry const* entr = GetDecisionEntry(filterName);

		if ( entr == ARTUS_CPP11_NULLPTR ){
			LOG_FATAL( "Decision entry with name " + filterName + " not found" );
		}
		return entr->second;
	}

	FilterDecisions const& GetFilterDecisions() const {
		return m_filterDecisions;
	}

	void SetFilterDecision(std::string filterName, bool passed) {
		Decision desc = passed ? Decision::Passed : Decision::NotPassed;
		DecisionEntry * entr = GetDecisionEntry(filterName);

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

	std::string ToString() const {
		std::stringstream s;
		s << "== Filter Decision == " << std::endl;

		for (FilterDecisions::const_iterator it = m_filterDecisions.begin();
				it != m_filterDecisions.end(); it++) {
			s << it->first << " : " << DecisionToString( it->second ) << std::endl;
		}

		return s.str();
	}

	std::string DecisionToString ( Decision dc ) const {
		if ( dc == Decision::Passed )
			return "passed";
		else if ( dc == Decision::NotPassed )
			return "not passed";
		else
			return "undefined";
	}

private:

	// optimize this, without strings
	FilterDecisions m_filterDecisions;

	mutable bool m_cacheHasPassed;
	mutable bool m_IsCachedHasPassed;
};
