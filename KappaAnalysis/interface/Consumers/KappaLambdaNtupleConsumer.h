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

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_X", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.X();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_Y", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.Y();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_Z", [](event_type const& event, product_type const& product)
		{
			return event.m_vertexSummary->pv.position.Z();
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
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("x1", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x1);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("x2", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x2);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("qScale", [bInpData](event_type const& event, product_type const& product)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->qScale);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("rho", [](event_type const& event, product_type const& product) {
			return event.m_pileupDensity->rho;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("PFMet", [](event_type const& event, product_type const& product) {
			return event.m_met->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genNPartons", [](event_type const& event, product_type const& product) {
			return product.m_genNPartons;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("NPFCandidates", [](event_type const& event, product_type const& product)
		{
			return event.m_packedPFCandidates->size();
		});
	
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genNJets", [](event_type const& event, product_type const& product) {
		       return event.m_genJets->size();
	       	});		

		LambdaNtupleConsumer<TTypes>::AddRMFLVQuantity("leadingGenJetLV", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 1 ? event.m_genJets->at(0).p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingGenJetPt", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 1 ? event.m_genJets->at(0).p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingGenJetEta", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 1 ? event.m_genJets->at(0).p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingGenJetPhi", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 1 ? event.m_genJets->at(0).p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingGenJetMass", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 1 ? event.m_genJets->at(0).p4.mass() : DefaultValues::UndefinedFloat;
		});
	
		LambdaNtupleConsumer<TTypes>::AddRMFLVQuantity("trailingGenJetLV", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 2 ? event.m_genJets->at(1).p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingGenJetPt", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 2 ? event.m_genJets->at(1).p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingGenJetEta", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 2 ? event.m_genJets->at(1).p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingGenJetPhi", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 2 ? event.m_genJets->at(1).p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingGenJetMass", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 2 ? event.m_genJets->at(1).p4.mass() : DefaultValues::UndefinedFloat;
		});
	
		LambdaNtupleConsumer<TTypes>::AddRMFLVQuantity("thirdGenJetLV", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 3 ? event.m_genJets->at(2).p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("thirdGenJetPt", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 3 ? event.m_genJets->at(2).p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("thirdGenJetEta", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 3 ? event.m_genJets->at(2).p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("thirdGenJetPhi", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 3 ? event.m_genJets->at(2).p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("thirdGenJetMass", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 3 ? event.m_genJets->at(2).p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<TTypes>::AddRMFLVQuantity("fourthGenJetLV", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 4 ? event.m_genJets->at(3).p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fourthGenJetPt", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 4 ? event.m_genJets->at(3).p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fourthGenJetEta", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 4 ? event.m_genJets->at(3).p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fourthGenJetPhi", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 4 ? event.m_genJets->at(3).p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fourthGenJetMass", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 4 ? event.m_genJets->at(3).p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<TTypes>::AddRMFLVQuantity("fifthGenJetLV", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 5 ? event.m_genJets->at(4).p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fifthGenJetPt", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 5 ? event.m_genJets->at(4).p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fifthGenJetEta", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 5 ? event.m_genJets->at(4).p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fifthGenJetPhi", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 5 ? event.m_genJets->at(4).p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("fifthGenJetMass", [](event_type const& event, product_type const& product) {
			return event.m_genJets->size() >= 5 ? event.m_genJets->at(4).p4.mass() : DefaultValues::UndefinedFloat;
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
			if ((boost::algorithm::icontains(quantity, "filter") || boost::algorithm::icontains(quantity, "cut")) &&
			   (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.fres (FilterResult).";
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
