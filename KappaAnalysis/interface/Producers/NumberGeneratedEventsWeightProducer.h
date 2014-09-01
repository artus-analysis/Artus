#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief NumberGeneratedEventsWeightProducer
   Config tags:
   - NumberGeneratedEvents, e.g. "1234"

*/


template<class TTypes>
class NumberGeneratedEventsWeightProducer : public ProducerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const {
		return "NumberGeneratedEventsWeightProducer";
	}

	virtual void Produce(event_type const& event,
	                     product_type & product,
	                     setting_type const& settings) const
	{
		product.m_weights["numberGeneratedEventsWeight"] = (1.0 / settings.GetNumberGeneratedEvents());
	}

};
