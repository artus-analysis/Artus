
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
