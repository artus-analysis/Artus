/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

class FilterResult {
public:

	enum class Decision { Undefined, Passed, NotPassed };

	typedef std::list < std::string> FilterNames;
	typedef std::map<std::string, Decision> FilterDecisions;



	FilterResult() :
			// has passed by default
			m_cacheHasPassed(true), m_IsCachedHasPassed(false) {
	}

	FilterResult(FilterNames const& initialFilterNames ) :
			// has passed by default
			m_cacheHasPassed(true), m_IsCachedHasPassed(false) {


		for ( FilterNames::const_iterator it = initialFilterNames.begin();
				it != initialFilterNames.end(); it ++ ) {
			m_filterDecision[ *it ] = Decision::Undefined;
		}
	}

	// Note: only call this, when all FilterDecisions have been added, as this result is cached
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
		return GetFilterDecisions().at(filterName);
	}

	FilterDecisions const& GetFilterDecisions() const {
		return m_filterDecision;
	}

	void SetFilterDecision(std::string filterName, bool passed) {
		if ( passed ) {
			m_filterDecision[filterName] = Decision::Passed;
		}
		else {
			m_filterDecision[filterName] = Decision::NotPassed;
		}
		
		// recompute the cached filter result, if the new filter
		// decision might change this
		if (m_IsCachedHasPassed && ( m_cacheHasPassed != passed ))
			m_IsCachedHasPassed = false;
	}

	std::string ToString() const {
		std::stringstream s;
		s << "== Filter Decision == " << std::endl;

		for (FilterDecisions::const_iterator it = m_filterDecision.begin();
				it != m_filterDecision.end(); it++) {
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
	FilterDecisions m_filterDecision;

	mutable bool m_cacheHasPassed;
	mutable bool m_IsCachedHasPassed;
};
