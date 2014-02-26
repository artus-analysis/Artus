/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

/**
   \brief Base class for GlobalProducers, which are run before the pipelines.
   
   This is a base class for GlobalProducers, which produce something before a pipeline that can be 
   used fro all pieplines in the following. 
*/

template<class TTypes>
class GlobalProducerBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual ~GlobalProducerBase() {

	}

	virtual void Init(global_setting_type const& globalSettings) {
	}

	/// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool ProduceGlobal(event_type const& event,
			product_type& product,
			global_setting_type const& globalSettings) const = 0;

	/// Must return a unique id of the producer.
	virtual std::string GetProducerId() = 0;
};

