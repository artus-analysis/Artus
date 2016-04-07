#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/Utility.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"



template<class TTypes>
class KappaLambdaNtupleConsumer: public LambdaNtupleConsumer<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	std::string GetConsumerId() const override
	{
		return "KappaLambdaNtupleConsumer";
	}

	void Init(setting_type const& settings) override
	{
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("input", [](event_type const& event, product_type const& product)
		{
			return static_cast<int>(event.m_input);
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
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("npuMean", [bInpData](event_type const& event, product_type const& product)
		{
			if (bInpData)
				return DefaultValues::UndefinedFloat;
			return static_cast<KGenEventInfo*>(event.m_eventInfo)->nPUMean;
		});

		LambdaNtupleConsumer<TTypes>::AddIntQuantity("npu", [bInpData](event_type const& event, product_type const& product)
		{
			if (bInpData)
				return DefaultValues::UndefinedInt;
			return static_cast<int>(static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU);
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
				LOG(INFO) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.m_weights or product.m_optionalWeights.";
				LambdaNtupleConsumer<TTypes>::AddFloatQuantity( quantity, [quantity](event_type const & event, product_type const & product)
				{
					return SafeMap::GetWithDefault(product.m_weights, quantity, SafeMap::GetWithDefault(product.m_optionalWeights, quantity, 1.0));
				} );
			}
			if ((boost::algorithm::icontains(quantity, "filter") || boost::algorithm::icontains(quantity, "cut")) &&
			   (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(quantity) == 0))
			{
				LOG(INFO) << "\tQuantity \"" << quantity << "\" is tried to be taken from prduct.fres (FilterResult).";
				LambdaNtupleConsumer<TTypes>::AddIntQuantity( quantity, [quantity](event_type const & event, product_type const & product)
				{
					if (product.fres.GetDecisionEntry(quantity) != nullptr)
					{
						return (product.fres.GetDecisionEntry(quantity)->filterDecision == FilterResult::Decision::Passed) ? 1 : 0;
					}
					return -1;
				} );
			}
		}
		// need to be called at last
		LambdaNtupleConsumer<TTypes>::Init(settings);
	}
};
