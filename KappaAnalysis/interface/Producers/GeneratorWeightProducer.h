#pragma once

#include "Artus/Core/interface/ProducerBase.h"

template<class TTypes>
class GeneratorWeightProducer : public ProducerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "GeneratorWeightProducer";
	}

	virtual void Produce(event_type const& event,
						product_type& product,
						setting_type const& settings) const ARTUS_CPP11_OVERRIDE
		{
			product.m_weights["generatorWeight"] = event.m_genEventMetadata->weight;
		}

};
