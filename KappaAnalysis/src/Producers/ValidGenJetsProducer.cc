
#include "Artus/KappaAnalysis/interface/Producers/ValidGenJetsProducer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <Math/VectorUtil.h>


ValidGenJetsProducer::ValidGenJetsProducer() :
	KappaProducerBase(),
	ValidPhysicsObjectTools<KappaTypes, KGenJet>(&KappaSettings::GetGenJetLowerPtCuts,
	                                             &KappaSettings::GetGenJetUpperAbsEtaCuts,
	                                             &KappaProduct::m_validGenJets)
{
}

std::string ValidGenJetsProducer::GetProducerId() const {
	return "ValidGenJetsProducer";
}

void ValidGenJetsProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	ValidPhysicsObjectTools<KappaTypes, KGenJet>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets20",[this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_validGenJets, 20.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets30",[this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_validGenJets, 30.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets50",[this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_validGenJets, 50.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets80",[this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_validGenJets, 80.0);
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("thirdGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fourthGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fifthGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("sixthGenJetLV", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.mass() : DefaultValues::UndefinedFloat;
	});
}

void ValidGenJetsProducer::Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const
{
	assert(event.m_genJets);
	
	for (std::vector<KGenJet>::iterator jet = event.m_genJets->begin();
	     jet != event.m_genJets->end(); ++jet)
	{
		bool validJet = true;
		
		// remove leptons from list of jets via simple DeltaR isolation
		for (std::vector<KGenParticle*>::iterator lepton = product.m_genLeptonsFromBosonDecay.begin();
		     lepton != product.m_genLeptonsFromBosonDecay.end(); ++lepton)
		{
			RMFLV* visibleP4 = &((*lepton)->p4);
			if (std::abs((*lepton)->pdgId) == DefaultValues::pdgIdTau)
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, *lepton, static_cast<KGenTau*>(nullptr));
				if (genTau)
				{
					visibleP4 = &(genTau->visible.p4);
				}
			}
			
			validJet = validJet && ROOT::Math::VectorUtil::DeltaR(jet->p4, *visibleP4) > settings.GetJetLeptonLowerDeltaRCut();
		}

		// kinematic cuts
		validJet = validJet && this->PassKinematicCuts(&(*jet), event, product);

		// check possible analysis-specific criteria
		validJet = validJet && AdditionalCriteria(&(*jet), event, product, settings);

		if (validJet)
		{
			product.m_validGenJets.push_back(&(*jet));
		}
		else
		{
			product.m_invalidGenJets.push_back(&(*jet));
		}
	}
}

bool ValidGenJetsProducer::AdditionalCriteria(KGenJet* jet, KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const
{
	return true;
}

