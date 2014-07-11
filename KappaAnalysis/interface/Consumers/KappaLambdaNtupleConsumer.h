
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Utility/interface/DefaultValues.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"



template<class TTypes>
class KappaLambdaNtupleConsumer: public LambdaNtupleConsumer<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE
	{
		return "lambda_ntuple";
	}
	
	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE
	{
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::Quantities["run"] = [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nRun;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["lumi"] = [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nLumi;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["event"] = [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nEvent;
		};
		
		LambdaNtupleConsumer<TTypes>::Quantities["npv"] = [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->nVertices;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["npu"] = [pipeline](event_type const& event, product_type const& product)
		{
			return (pipeline->GetSettings().GetInputIsData() ?
			        DefaultValues::UndefinedFloat :
			        static_cast<KGenEventMetadata*>(event.m_eventMetadata)->numPUInteractionsTruth);
		};
		
		LambdaNtupleConsumer<TTypes>::Quantities["rho"] = [](event_type const& event, product_type const& product) {
			return event.m_jetArea->median;
		};
		
		// loop over all quantities containing "weight" (case-insensitive)
		// and try to find them in the weights map to write them out
		for (auto const & quantity : pipeline->GetSettings().GetQuantities())
		{
			if (boost::algorithm::icontains(quantity, "weight"))
			{
				if (LambdaNtupleConsumer<TTypes>::Quantities.count(quantity) == 0)
				{
					LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.m_weights.";
					LambdaNtupleConsumer<TTypes>::Quantities[quantity] = [quantity](event_type const & event, product_type const & product)
					{
						return SafeMap::GetWithDefault(product.m_weights, quantity, 1.0);
					};
				}
			}
		}
	
		// need to be called at last
		LambdaNtupleConsumer<TTypes>::Init(pipeline);
	}
};
