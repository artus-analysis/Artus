
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumerBase.h"



template<class TTypes>
class KappaLambdaNtupleConsumer: public LambdaNtupleConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;

	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda;

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE
	{
		return "lambda_ntuple";
	}
	
	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		this->m_valueExtractorMap["run"] = [](event_type const& event, product_type const& product) { return event.m_eventMetadata->nRun; };
		this->m_valueExtractorMap["lumi"] = [](event_type const& event, product_type const& product) { return event.m_eventMetadata->nLumi; };
		this->m_valueExtractorMap["event"] = [](event_type const& event, product_type const& product) { return event.m_eventMetadata->nEvent; };
		
		this->m_valueExtractorMap["npv"] = [](event_type const& event, product_type const& product) { return event.m_vertexSummary->nVertices; };
	
		// need to be called at last
		LambdaNtupleConsumerBase<TTypes>::Init(pipeline);
	}
};
