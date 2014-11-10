
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/Utility.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"



template<class TTypes>
class KappaLambdaNtupleConsumer: public LambdaNtupleConsumer<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE
	{
		return "KappaLambdaNtupleConsumer";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("run", [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nRun;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("lumi", [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nLumi;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("event", [](event_type const& event, product_type const& product)
		{
			return event.m_eventMetadata->nEvent;
		});
		
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("npv", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->nVertices;
		});

		bool bInpData = settings.GetInputIsData();
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("npu", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData ?
			        DefaultValues::UndefinedInt :
			        static_cast<KGenEventMetadata*>(event.m_eventMetadata)->numPUInteractionsTruth);
		});
		
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("rho", [](event_type const& event, product_type const& product) {
			return event.m_jetArea->median;
		});
		
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genDiLeptonDecayMode", [](event_type const& event, product_type const& product) {
			return Utility::ToUnderlyingValue(product.m_genDiLeptonDecayMode);
		});
		
		// loop over all quantities containing "weight" (case-insensitive)
		// and try to find them in the weights map to write them out
		for (auto const & quantity : settings.GetQuantities())
		{
			if (boost::algorithm::icontains(quantity, "weight") &&
			    (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(quantity) == 0) &&
			    (LambdaNtupleConsumer<TTypes>::GetDoubleQuantities().count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.m_weights or product.m_optionalWeights.";
				LambdaNtupleConsumer<TTypes>::AddFloatQuantity( quantity, [quantity](event_type const & event, product_type const & product)
				{
					return SafeMap::GetWithDefault(product.m_weights, quantity, SafeMap::GetWithDefault(product.m_optionalWeights, quantity, 1.0));
				} );
			}
		}

		// need to be called at last
		LambdaNtupleConsumer<TTypes>::Init(settings);
	}
};
