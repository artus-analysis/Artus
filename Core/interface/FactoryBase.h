/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <list>
#include <boost/noncopyable.hpp>

#include "Cpp11Support.h"

#include "ProducerBase.h"
#include "ConsumerBase.h"
#include "FilterBase.h"

// producer

// filter

// consumer
#include "Artus/Consumer/interface/CutFlowHistogramConsumer.h"


/*template<class TTypes>*/
class FactoryBase: public boost::noncopyable {
public:

	virtual ~FactoryBase() {
	}

	// these virtual methods offer no default objects, so far
	virtual ProducerBaseUntemplated * createProducer ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

	virtual FilterBaseUntemplated * createFilter ( std::string const& id ) {
		return ARTUS_CPP11_NULLPTR;
	}

	virtual ConsumerBaseUntemplated * createConsumer ( std::string const& id ) {
		/*if(id == CutFlowHistogramConsumer<TTypes>().GetConsumerId())
			return new CutFlowHistogramConsumer<TTypes>();
		else*/
			return ARTUS_CPP11_NULLPTR;
	}

};
