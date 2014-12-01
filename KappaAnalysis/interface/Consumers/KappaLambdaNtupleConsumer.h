
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
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("input", [](event_type const& event, product_type const& product) -> int
		{
			return event.m_input;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("run", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nRun;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("lumi", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nLumi;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("event", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nEvent;
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
			        static_cast<KGenEventInfo*>(event.m_eventInfo)->nPUMean);
		});
		
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("rho", [](event_type const& event, product_type const& product) {
			return event.m_pileupDensity->rho;
		});
		
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genDiLeptonDecayMode", [](event_type const& event, product_type const& product) {
			return Utility::ToUnderlyingValue(product.m_genDiLeptonDecayMode);
		});
		
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genNPartons", [](event_type const& event, product_type const& product) {
			return product.m_genNPartons;
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
