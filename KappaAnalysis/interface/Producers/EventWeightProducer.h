
#pragma once

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief EventWeightProducer (can only run locally)
   Config tags:
   - EventWeight, e.g. "eventWeight"
   
   Multiplies all entries in the map product.m_weights
   and writes the result to product.m_weights[settings.GetEventWeight()]
   
   By adding the weight quantity names to the Quantity config setting,
   they will be individually written to the ntuple by the LambdaNtupleConsumer
*/

template<class TTypes>
class EventWeightProducer: public ProducerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "event_weight";
	}
	
	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		double eventWeight = 1.0;
	
		// loop over all previously calculated weights and multiply them
		for(std::map<std::string, double>::const_iterator weight = product.m_weights.begin();
			weight != product.m_weights.end(); ++weight)
		{
			eventWeight *= weight->second;
		}
	
		product.m_weights[settings.GetEventWeight()] = eventWeight;
	}
};


