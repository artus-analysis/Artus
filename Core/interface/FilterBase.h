/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/ProcessNodeBase.h"
#include "Artus/Utility/interface/ArtusLogging.h"

// forward define to be able to use the event pipeline here
template<class TTypes>
class Pipeline;

template<class TTypes>
class FilterBase: public ProcessNodeBase {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~FilterBase() {
	}

	// initialise global pre-filters
	virtual void Init(setting_type const& settings) {
		LOG(INFO) << "Initialize global Filter \"" << this->GetFilterId() << "\".";
	}

	/*
	 * Must return a unique id of the filter.
	 */
	virtual std::string GetFilterId() const = 0;

	// process global event
	virtual bool DoesEventPass(event_type const& event,
			product_type const& product, setting_type const& globalSettings) const
	{
		LOG(FATAL) << "DoesEventPassGlobal function for filter \"" << this->GetFilterId() << "\" is not implemented!";
		return false;
	}

	virtual std::string ToString(bool bVerbose = false) {
		return GetFilterId();
	}

	virtual ProcessNodeType GetProcessNodeType () const
		ARTUS_CPP11_OVERRIDE
		ARTUS_CPP11_FINAL
	{
		return ProcessNodeType::Filter;
	}

};
