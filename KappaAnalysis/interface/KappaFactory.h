
#pragma once

#include "Artus/Core/interface/FactoryBase.h"


class KappaFactory: public FactoryBase {
public:

	KappaFactory();
	virtual ~KappaFactory();

	ProducerBaseUntemplated* createProducer(std::string const& id) override;
	FilterBaseUntemplated* createFilter(std::string const& id) override;
	ConsumerBaseUntemplated* createConsumer(std::string const& id) override;

};
