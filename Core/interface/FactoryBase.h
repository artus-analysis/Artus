
#pragma once

#include <boost/noncopyable.hpp>

#include "ProducerBase.h"
#include "ConsumerBase.h"
#include "FilterBase.h"

class FactoryBase: public boost::noncopyable {
public:

	virtual ~FactoryBase();

	// these virtual methods offer no default objects, so far
	virtual ProducerBaseUntemplated * createProducer ( std::string const& id );
	virtual FilterBaseUntemplated * createFilter ( std::string const& id );
	virtual ConsumerBaseUntemplated * createConsumer ( std::string const& id );

};
