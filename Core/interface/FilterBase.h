/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

// forward define to be able to use the event pipeline here
template<class TTypes>
class Pipeline;

template<class TTypes>
class FilterBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~FilterBase() {
	}

	// todo: do we event need this init this ?
	virtual void Init(Pipeline<TTypes> * pset) {
		//m_pipeline = pset;
	}

	virtual void Finish() {
	}

	/*
	 * Must return a unique id of the filter.
	 */
	virtual std::string GetFilterId() = 0;

	virtual bool DoesEventPass(event_type const& event,
			product_type const& product, setting_type const& settings) = 0;

	virtual std::string ToString(bool bVerbose = false) {
		return GetFilterId();
	}

};
