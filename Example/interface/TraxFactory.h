/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/FactoryBase.h"
#include "TraxTypes.h"


class TraxFactory: public FactoryBase {
public:

	TraxFactory();
	virtual ~TraxFactory();

	ProducerBaseUntemplated* createProducer(std::string const& id) override;
	ConsumerBaseUntemplated* createConsumer(std::string const& id) override;
	FilterBaseUntemplated* createFilter(std::string const& id) override;
};
