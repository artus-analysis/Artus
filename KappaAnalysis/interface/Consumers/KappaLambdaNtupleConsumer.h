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
	typedef typename TTypes::metadata_type metadata_type;

	std::string GetConsumerId() const override
	{
		return "KappaLambdaNtupleConsumer";
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "input", [](event_type const& event, product_type const& product)
		{
			return static_cast<int>(event.m_input);
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity(metadata, "run", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nRun;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity(metadata, "lumi", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nLumi;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity(metadata, "event", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nEvent;
		});		
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity(metadata, "nbx", [](event_type const& event, product_type const& product) -> uint64_t
		{
			return event.m_eventInfo->nBX;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "npv", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->nVertices;
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "firstPV_X", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.X();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "firstPV_Y", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.Y();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "firstPV_Z", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.Z();
		});

		bool bInpData = settings.GetInputIsData();
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "npuMean", [bInpData](event_type const& event, product_type const& product)
		{
			if (bInpData)
				return DefaultValues::UndefinedFloat;
			return static_cast<KGenEventInfo*>(event.m_eventInfo)->nPUMean;
		});

		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "npu", [bInpData](event_type const& event, product_type const& product)
		{
			if (bInpData)
				return DefaultValues::UndefinedInt;
			return static_cast<int>(static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "x1", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x1);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "x2", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x2);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "qScale", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->qScale);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "rho", [](event_type const& event, product_type const& product) {
			return event.m_pileupDensity->rho;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata, "PFMet", [](event_type const& event, product_type const& product) {
			return event.m_met->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "genNPartons", [](event_type const& event, product_type const& product) {
			return product.m_genNPartons;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "NPFCandidates", [](event_type const& event, product_type const& product)
		{
			return event.m_packedPFCandidates->size();
		});

		LambdaNtupleConsumer<TTypes>::AddCartesianRMFLVQuantity(metadata, "LHE_p_1", [](event_type const& event, product_type const& product) -> CartesianRMFLV
		{
			return event.m_lheParticles->particles.at(3).p4;
		});
		
		LambdaNtupleConsumer<TTypes>::AddCartesianRMFLVQuantity(metadata, "LHE_p_2", [](event_type const& event, product_type const& product) -> CartesianRMFLV
		{
			return (event.m_lheParticles->particles.size() >= 5) ? event.m_lheParticles->particles.at(4).p4 : DefaultValues::UndefinedCartesianRMFLV;
		});
		
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "LHE_SPINUP_pos_lepton", [](event_type const& event, product_type const& product)
		{
			for (std::vector<KLHEParticle>::iterator lheParticle = event.m_lheParticles->particles.begin();
			     lheParticle != event.m_lheParticles->particles.end(); ++lheParticle)
			{
				if ((lheParticle->pdgId == -DefaultValues::pdgIdElectron) ||
				    (lheParticle->pdgId == -DefaultValues::pdgIdMuon) ||
				    (lheParticle->pdgId == -DefaultValues::pdgIdTau))
				{
					return (lheParticle->spinInfo > 0 ? +1 : -1);
				}
			}
			return DefaultValues::UndefinedInt;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata, "LHE_SPINUP_neg_lepton", [](event_type const& event, product_type const& product)
		{
			for (std::vector<KLHEParticle>::iterator lheParticle = event.m_lheParticles->particles.begin();
			     lheParticle != event.m_lheParticles->particles.end(); ++lheParticle)
			{
				if (((lheParticle->pdgId == DefaultValues::pdgIdElectron) ||
				     (lheParticle->pdgId == DefaultValues::pdgIdMuon) ||
				     (lheParticle->pdgId == DefaultValues::pdgIdTau)) && (lheParticle->status == 1))
				{
					return (lheParticle->spinInfo > 0 ? +1 : -1);
				}
			}
			return DefaultValues::UndefinedInt;
		});

		// loop over all quantities containing "weight" (case-insensitive)
		// and try to find them in the weights map to write them out
		for (std::string const& quantity : settings.GetQuantities())
		{
			if (boost::algorithm::icontains(quantity, "weight") &&
			    (metadata.m_commonFloatQuantities.count(quantity) == 0) &&
			    (metadata.m_commonDoubleQuantities.count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.m_weights or product.m_optionalWeights.";
				LambdaNtupleConsumer<TTypes>::AddFloatQuantity(metadata,  quantity, [quantity](event_type const & event, product_type const & product)
				{
					return SafeMap::GetWithDefault(product.m_weights, quantity, SafeMap::GetWithDefault(product.m_optionalWeights, quantity, 1.0));
				} );
			}
			if ((boost::algorithm::icontains(quantity, "filter") || boost::algorithm::icontains(quantity, "cut")) &&
			   (metadata.m_commonFloatQuantities.count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.fres (FilterResult).";
				LambdaNtupleConsumer<TTypes>::AddIntQuantity(metadata,  quantity, [quantity](event_type const & event, product_type const & product)
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
		LambdaNtupleConsumer<TTypes>::Init(settings, metadata);
	}
};
