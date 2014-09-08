
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/FactoryBase.h"

//template<class TTypes>

// possible problems:
// the static storage of the GenTauDecayProducer

class KappaFactory: public FactoryBase/*<TTypes>*/ {
public:

	KappaFactory() : FactoryBase/*<TTypes>*/() {
	}

	virtual ~KappaFactory() {}

	virtual ProducerBaseUntemplated * createProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE;

	virtual FilterBaseUntemplated * createFilter ( std::string const& id )
		ARTUS_CPP11_OVERRIDE;

	virtual ConsumerBaseUntemplated * createConsumer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE;

};
