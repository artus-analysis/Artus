
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief EventWeightProducer (can only run locally)
   Config tags:
   - EventWeight, e.g. "eventWeight"
   
   Multiplies all entries in the map product.m_weights with m_baseWeight
   and writes the result to product.m_weights[settings.GetEventWeight()]
   
   By adding the weight quantity names to the Quantity config setting,
   they will be individually written to the ntuple by the LambdaNtupleConsumer
*/

class EventWeightProducer: public KappaProducerBase {
public:

	std::string GetProducerId() const override;
	
	~EventWeightProducer();
	
	void Init(setting_type const& settings, metadata_type& metadata) override;
	
	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;


private:
	std::string pipelineName;
	mutable std::vector<std::string> m_weightNames;
};

