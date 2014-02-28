/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <list>
#include <boost/noncopyable.hpp>

#include "Cpp11Support.h"

#include "GlobalProducerBase.h"
#include "LocalProducerBase.h"
#include "ConsumerBase.h"
#include "FilterBase.h"


template<class TTypes>
class FactoryBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef GlobalProducerBase< TTypes > global_producer_base_type;
	typedef LocalProducerBase< TTypes > local_producer_base_type;
	typedef ConsumerBase< TTypes > consumer_base_type;
	typedef FilterBase< TTypes > filter_base_type;

	virtual ~FactoryBase() {
	}

	// these virtual methods offer no default objects, so far
	virtual global_producer_base_type * createGlobalProducer ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

	virtual local_producer_base_type * createLocalProducer ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

	virtual consumer_base_type * createConsumer ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

	virtual filter_base_type * createFilter ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

};
