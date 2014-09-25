
#include "Artus/KappaAnalysis/interface/Producers/ValidBTaggedJetsProducer.h"


std::string ValidBTaggedJetsProducer::GetProducerId() const {
	return "ValidBTaggedJetsProducer";
}

void ValidBTaggedJetsProducer::Init(KappaSettings const& settings)
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

void ValidBTaggedJetsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
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
			
			//entry point for promotion/demotion of btagged jets - this is recommanded from bPOG
			if (settings.GetApplyBTagSF() && !settings.GetInputIsData())
			{
				int jetflavor = 1;
				for (auto iterator=product.m_genMatchedJets.begin(); iterator!=product.m_genMatchedJets.end(); ++iterator)
				  {
				    if ( iterator->first->p4 == tjet->p4 )
				      {
					jetflavor = std::abs(iterator->second->pdgId());
					LOG(DEBUG) << "Jet " << iterator->first->p4 << "  => " << iterator->second->p4;
					LOG(DEBUG) << "particle ID " << std::abs(iterator->second->pdgId());
				      }		
				  }
				unsigned int btagSys = BtagSF::kNo;
				unsigned int bmistagSys = BtagSF::kNo;
				bool is8TeV = true;
				if (settings.GetBTagShift()<0)
				        btagSys = BtagSF::kDown;
				if (settings.GetBTagShift()>0)
				        btagSys = BtagSF::kUp;
				if (settings.GetBMistagShift()<0)
				        bmistagSys = BtagSF::kDown;
				if (settings.GetBMistagShift()>0)
				        bmistagSys = BtagSF::kUp;
				bool before = validBJet;
			  BtagSF bla;
			  validBJet = bla.isbtagged(tjet->p4.pt(), tjet->p4.eta(), combinedSecondaryVertex, jetflavor, settings.GetInputIsData(), btagSys, bmistagSys, is8TeV);
			  if (before != validBJet) 
			        LOG(DEBUG) << "Promoted/demoted : " << validBJet;
			}

			if (validBJet)
				product.m_bTaggedJets.push_back(tjet);
			else
				product.m_nonBTaggedJets.push_back(tjet);
		}
}

bool ValidBTaggedJetsProducer::AdditionalCriteria(KDataPFTaggedJet* jet, KappaEvent const& event,
                                KappaProduct& product, KappaSettings const& settings) const
{
	bool validBJet = true;
	return validBJet;
}

