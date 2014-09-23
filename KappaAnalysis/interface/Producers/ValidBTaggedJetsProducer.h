
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"


/**
   \brief Producer for valid b-tagged jets.

   Exploits the properties of b-tagged jets
*/
class ValidBTaggedJetsProducer: public KappaProducerBase
{

public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ValidBTaggedJetsProducer";
	}

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE
	{
		KappaProducerBase::Init(settings);
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("nBJets", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_bTaggedJets.size();
		});
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("nBJets20", [this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 20.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("nBJets30", [this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 30.0);
		});		
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("bJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Pt() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("bJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Eta() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<KappaTypes>::AddQuantity("bJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Phi() : DefaultValues::UndefinedDouble;
		});
	}

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (std::vector<KDataPFJet*>::iterator jet = product.m_validJets.begin();
				 jet != product.m_validJets.end(); ++jet)
			{
				bool validBJet = true;
				KDataPFTaggedJet* tjet = static_cast<KDataPFTaggedJet*>(*jet);

				float combinedSecondaryVertex = tjet->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerMetadata);

				if (combinedSecondaryVertex < settings.GetBTaggedJetCombinedSecondaryVertexMediumWP() ||
					std::abs(tjet->p4.eta()) > settings.GetBTaggedJetAbsEtaCut()) {
					validBJet = false;
				}

				validBJet = validBJet && AdditionalCriteria(tjet, event, product, settings);

				if (validBJet)
					product.m_bTaggedJets.push_back(tjet);
				else
					product.m_nonBTaggedJets.push_back(tjet);
			}
	}


protected:

	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
		bool validBJet = true;
		return validBJet;
	}

};
