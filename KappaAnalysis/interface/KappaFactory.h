
#pragma once

#include "Artus/Core/interface/FactoryBase.h"

//template<class TTypes>

// possible problems:
// the static storage of the GenTauDecayProducer

class KappaFactory: public FactoryBase/*<TTypes>*/ {
public:

	KappaFactory() : FactoryBase/*<TTypes>*/() {
	}

	virtual ~KappaFactory() {}

	virtual ProducerBaseUntemplated* createProducer(std::string const& id) override;

	virtual FilterBaseUntemplated* createFilter(std::string const& id) override;

	virtual ConsumerBaseUntemplated* createConsumer(std::string const& id) override;

};
