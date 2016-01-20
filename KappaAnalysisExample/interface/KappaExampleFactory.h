
#pragma once

#include "Artus/KappaAnalysis/interface/KappaFactory.h"


class KappaExampleFactory: public KappaFactory {
public:
	virtual ProducerBaseUntemplated* createProducer(std::string const& id) override;
	virtual FilterBaseUntemplated* createFilter(std::string const& id) override;
	virtual ConsumerBaseUntemplated* createConsumer(std::string const& id) override;

};
