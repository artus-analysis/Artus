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

	virtual std::string GetProducerId() const {
		return "NumberGeneratedEventsWeightProducer";
	}

	virtual void Produce(KappaEvent const& event,
	                     KappaProduct & product,
	                     KappaSettings const& settings) const
	{
		product.m_weights["numberGeneratedEventsWeight"] = (1.0 / settings.GetNumberGeneratedEvents());
	}

};
