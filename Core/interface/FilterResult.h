/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

class FilterResult {
public:

	typedef std::map<std::string, bool> FilterDecisions;

	FilterResult() :
			// has passed by default
			m_cacheHasPassed(true), m_IsCachedHasPassed(false) {
	}

	// Note: only call this, when all FilterDecisions have been added, as this result is cached
	bool HasPassed() const {
		if (m_IsCachedHasPassed)
			return m_cacheHasPassed;

		m_cacheHasPassed = true;
		for (FilterDecisions::const_iterator it = GetFilterDecisions().begin();
				it != GetFilterDecisions().end(); it++) {
			if (it->second == false)
				m_cacheHasPassed = false;
		}

		m_IsCachedHasPassed = true;
		return m_cacheHasPassed;
	}

	bool HasPassedIfExcludingFilter(std::string const& excludedFilter) const {
		for (FilterDecisions::const_iterator it = GetFilterDecisions().begin();
				it != GetFilterDecisions().end(); it++) {
			if (it->second == false)
				if (it->first != excludedFilter)
					return false;
		}

		return true;
	}

	bool GetFilterDecision(std::string filterName) const {
		return GetFilterDecisions().at(filterName);
	}

	FilterDecisions const& GetFilterDecisions() const {
		return m_filterDecision;
	}

	void SetFilterDecisions(std::string filterName, bool passed) {
		m_filterDecision[filterName] = passed;
		
		if (!passed && m_IsCachedHasPassed && m_cacheHasPassed)
			m_IsCachedHasPassed = false;
	}

	std::string ToString() const {
		std::stringstream s;
		s << "== Filter Decision == " << std::endl;

		for (FilterDecisions::const_iterator it = m_filterDecision.begin();
				it != m_filterDecision.end(); it++) {
			s << it->first << " : " << it->second << std::endl;
		}

		return s.str();
	}

private:

	// optimize this, without strings
	FilterDecisions m_filterDecision;

	mutable bool m_cacheHasPassed;
	mutable bool m_IsCachedHasPassed;
};
