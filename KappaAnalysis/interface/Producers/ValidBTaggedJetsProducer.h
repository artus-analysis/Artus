
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"
#include "Artus/KappaAnalysis/interface/Utility/BtagSF.hh"


/**
   \brief Producer for valid b-tagged jets.

   Exploits the properties of b-tagged jets
*/
template<class TTypes>
class ValidBTaggedJetsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ValidBTaggedJetsProducer";
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::Quantities["nBJets"] = [](event_type const& event, product_type const& product) {
			return product.m_bTaggedJets.size();
		};
		LambdaNtupleConsumer<TTypes>::Quantities["nBJets20"] = [this](event_type const& event, product_type const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 20.0);
		};
		LambdaNtupleConsumer<TTypes>::Quantities["nBJets30"] = [this](event_type const& event, product_type const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 30.0);
		};
		
		LambdaNtupleConsumer<TTypes>::Quantities["bJetPt"] = [](event_type const& event, product_type const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Pt() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["bJetEta"] = [](event_type const& event, product_type const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Eta() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["bJetPhi"] = [](event_type const& event, product_type const& product) {
			return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Phi() : DefaultValues::UndefinedDouble;
		};
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
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
				if (settings.GetApplyBTagSF() && !settings.GetInputIsData()){
				        int jetflavor = 1;
					//if (isPartonExist[iJ] && !datasetID.isData()) jetflavor = TMath::Abs(partonFlavor[iJ]) //gen particle must be loaded
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


protected:

	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validBJet = true;
		return validBJet;
	}

};
