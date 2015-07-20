
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief EventWeightProducer (can only run locally)
   Config tags:
   - EventWeight, e.g. "eventWeight"
   
   Multiplies all entries in the map product.m_weights
   and writes the result to product.m_weights[settings.GetEventWeight()]
   
   By adding the weight quantity names to the Quantity config setting,
   they will be individually written to the ntuple by the LambdaNtupleConsumer
*/

class EventWeightProducer: public KappaProducerBase {
public:

	virtual std::string GetProducerId() const override;
	
	~EventWeightProducer();
	
	virtual void Init(KappaSettings const& settings) override;
	
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;


private:
	std::string pipelineName;
	mutable std::vector<std::string> m_weightNames;
};

