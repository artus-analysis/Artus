#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief NumberGeneratedEventsWeightProducer
   Config tags:
   - NumberGeneratedEvents, e.g. "1234"

*/

class NumberGeneratedEventsWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event,
	                     KappaProduct & product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;

};
