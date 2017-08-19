
#include "Artus/KappaAnalysis/interface/Producers/ValidGenJetsProducer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <Math/VectorUtil.h>


ValidGenJetsProducer::ValidGenJetsProducer() :
	KappaProducerBase(),
	ValidPhysicsObjectTools<KappaTypes, KGenJet>(&KappaTypes::setting_type::GetGenJetLowerPtCuts,
	                                             &KappaTypes::setting_type::GetGenJetUpperAbsEtaCuts,
	                                             &KappaTypes::product_type::m_validGenJets)
{
}

std::string ValidGenJetsProducer::GetProducerId() const {
	return "ValidGenJetsProducer";
}

void ValidGenJetsProducer::Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);
	ValidPhysicsObjectTools<KappaTypes, KGenJet>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets20",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validGenJets, 20.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets30",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validGenJets, 30.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets50",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validGenJets, 50.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nGenJets80",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validGenJets, 80.0);
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 1 ? product.m_validGenJets.at(0)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 2 ? product.m_validGenJets.at(1)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("thirdGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 3 ? product.m_validGenJets.at(2)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fourthGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 4 ? product.m_validGenJets.at(3)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fifthGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 5 ? product.m_validGenJets.at(4)->p4.mass() : DefaultValues::UndefinedFloat;
	});

	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("sixthGenJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4 : DefaultValues::UndefinedRMFLV;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthGenJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
		return product.m_validGenJets.size() >= 6 ? product.m_validGenJets.at(5)->p4.mass() : DefaultValues::UndefinedFloat;
	});
}

void ValidGenJetsProducer::Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
                                   KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const
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
		validJet = validJet && AdditionalCriteria(&(*jet), event, product, settings, metadata);

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

bool ValidGenJetsProducer::AdditionalCriteria(KGenJet* jet, KappaTypes::event_type const& event, KappaTypes::product_type& product,
                                              KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const
{
	return true;
}

