
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"

/**
   \brief Producer for valid jets.
   
   Valid jets pass the particle flow JetID ("tight", "medium" or "loose") by JetMET. There are no
   pileupJetID requirements applied at the moment.

   This producer requires the "JetID" config tag to be set to "tight", "medium" or "loose".
   
   This producer should be run after the ValidElectronsProducer, ValidMuonsProducer and ValidTausProducer,
   because it cleans the list of jets according to the valid leptons.
   
   This is a templated base version. Use the actual versions for KDataPFJets or KDataPFTaggedJets
   at the end of this file.
*/


template<class TJet, class TValidJet>
class ValidJetsProducerBase: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, TValidJet>
{

public:

	enum class ValidJetsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	static ValidJetsInput ToValidJetsInput(std::string const& validJetsInput)
	{
		if (validJetsInput == "uncorrected") return ValidJetsInput::UNCORRECTED;
		else if (validJetsInput == "corrected") return ValidJetsInput::CORRECTED;
		else return ValidJetsInput::AUTO;
	}

	enum class JetID : int
	{
		NONE  = -1,
		TIGHT = 0,
		MEDIUM = 1,
		LOOSE  = 2,
	};
	static JetID ToJetID(std::string const& jetID)
	{
		if (jetID == "tight") return JetID::TIGHT;
		else if (jetID == "medium") return JetID::MEDIUM;
		else if (jetID == "loose") return JetID::LOOSE;
		else return JetID::NONE;
	}
	
	ValidJetsProducerBase(std::vector<TJet>* KappaEvent::*jets,
	                      std::vector<std::shared_ptr<TJet> > KappaProduct::*correctJets,
	                      std::vector<TValidJet*> KappaProduct::*validJets) :
		KappaProducerBase(),
		ValidPhysicsObjectTools<KappaTypes, TValidJet>(&KappaSettings::GetJetLowerPtCuts,
		                                               &KappaSettings::GetJetUpperAbsEtaCuts,
		                                               validJets),
		m_jetsMember(jets),
		m_correctedJetsMember(correctJets)
	{
	}

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE
	{
		KappaProducerBase::Init(settings);
		ValidPhysicsObjectTools<KappaTypes, TValidJet>::Init(settings);
		
		validJetsInput = ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
		jetID = ToJetID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID())));
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::Quantities["nJets"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size();
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["nJets20"] = [this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 20.0);
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["nJets30"] = [this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		};
		
		LambdaNtupleConsumer<KappaTypes>::Quantities["leadingJetPt"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Pt() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["leadingJetEta"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Eta() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["leadingJetPhi"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Phi() : DefaultValues::UndefinedDouble;
		};
		
		LambdaNtupleConsumer<KappaTypes>::Quantities["trailingJetPt"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Pt() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["trailingJetEta"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Eta() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["trailingJetPhi"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Phi() : DefaultValues::UndefinedDouble;
		};
	}

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// select input source
		std::vector<TJet*> jets;
		if ((validJetsInput == ValidJetsInput::AUTO && ((product.*m_correctedJetsMember).size() > 0)) || (validJetsInput == ValidJetsInput::CORRECTED))
		{
			jets.resize((product.*m_correctedJetsMember).size());
			size_t jetIndex = 0;
			for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
			     jet != (product.*m_correctedJetsMember).end(); ++jet)
			{
				jets[jetIndex] = jet->get();
				++jetIndex;
			}
		}
		else
		{
			jets.resize((event.*m_jetsMember)->size());
			size_t jetIndex = 0;
			for (typename std::vector<TJet>::iterator jet = (event.*m_jetsMember)->begin(); jet != (event.*m_jetsMember)->end(); ++jet)
			{
				jets[jetIndex] = &(*jet);
				++jetIndex;
			}
		}
		
		for (typename std::vector<TJet*>::iterator jet = jets.begin(); jet != jets.end(); ++jet)
		{
			bool validJet = true;

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// jets, all eta
			float maxNeutralFraction = this->GetMaxNeutralFraction();
			
			validJet = validJet
					   && (*jet)->neutralHadFraction + (*jet)->HFHadFraction < maxNeutralFraction
					   && (*jet)->neutralEMFraction < maxNeutralFraction
					   && (*jet)->nConst > 1;
			// jets, |eta| < 2.4 (tracker)
			if (std::abs((*jet)->p4.eta()) < 2.4)
			{
				validJet = validJet
						   && (*jet)->chargedHadFraction > 0.0
						   && (*jet)->nCharged > 0
						   && (*jet)->chargedEMFraction < 0.99;
			}
			
			// remove leptons from list of jets via simple DeltaR isolation
			for (std::vector<KLepton*>::const_iterator lepton = product.m_validLeptons.begin();
			     validJet && lepton != product.m_validLeptons.end(); ++lepton)
			{
				validJet = validJet && ROOT::Math::VectorUtil::DeltaR((*jet)->p4, (*lepton)->p4) > settings.GetJetLeptonLowerDeltaRCut();
			}
			
			// kinematic cuts
			validJet = validJet && this->PassKinematicCuts(*jet, event, product);
			
			// check possible analysis-specific criteria
			validJet = validJet && AdditionalCriteria(*jet, event, product, settings);

			if (validJet)
			{
				product.m_validJets.push_back(*jet);
			}
			else
			{
				product.m_invalidJets.push_back(*jet);
			}
		}
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
		bool validJet = true;
		
		return validJet;
	}


private:
	std::vector<TJet>* KappaEvent::*m_jetsMember;
	std::vector<std::shared_ptr<TJet> > KappaProduct::*m_correctedJetsMember;
	
	ValidJetsInput validJetsInput;
	JetID jetID;

	float GetMaxNeutralFraction() const
	{
		float maxNeutralFraction = 0.0;
		
		if (jetID == JetID::TIGHT)
		{
			maxNeutralFraction = 0.90;
		}
		else if (jetID == JetID::MEDIUM)
		{
			maxNeutralFraction = 0.95;
		}
		else if (jetID == JetID::LOOSE)
		{
			maxNeutralFraction = 0.99;
		}
		else if (jetID != JetID::NONE)
		{
			LOG(FATAL) << "Jet ID of type " << Utility::ToUnderlyingValue(jetID) << " not yet implemented!";
		}
		
		return maxNeutralFraction;
	}

};



/**
   \brief Producer for valid jets (simple PF jets).
   
   Operates on the vector event.m_jets.
*/
class ValidJetsProducer: public ValidJetsProducerBase<KDataPFJet, KDataPFJet>
{
public:
	ValidJetsProducer() : ValidJetsProducerBase<KDataPFJet, KDataPFJet>(&KappaEvent::m_jets,
	                                                                            &KappaProduct::m_correctedJets,
	                                                                            &KappaProduct::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ValidJetsProducer";
	}
};



/**
   \brief Producer for valid jets (tagged PF jets).
   
   Operates on the vector event.m_tjets.
   
   Required config tags:
   - PuJetIDs
   - JetTaggerLowerCuts
*/
class ValidTaggedJetsProducer: public ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>
{
public:
	
	ValidTaggedJetsProducer() : ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>(&KappaEvent::m_tjets,
	                                                                                        &KappaProduct::m_correctedTaggedJets,
	                                                                                        &KappaProduct::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ValidTaggedJetsProducer";
	}

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE
	{
		ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>::Init(settings);
		
		puJetIdsByIndex = Utility::ParseMapTypes<size_t, std::string>(
				Utility::ParseVectorToMap(settings.GetPuJetIDs()),
				puJetIdsByHltName
		);
		
		jetTaggerLowerCutsByTaggerName = Utility::ParseMapTypes<std::string, float>(
				Utility::ParseVectorToMap(settings.GetJetTaggerLowerCuts()),
				jetTaggerLowerCutsByTaggerName
		);
		
		jetTaggerUpperCutsByTaggerName = Utility::ParseMapTypes<std::string, float>(
				Utility::ParseVectorToMap(settings.GetJetTaggerUpperCuts()),
				jetTaggerUpperCutsByTaggerName
		);
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::Quantities["leadingJetCSV"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(0))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["leadingJetTCHE"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(0))->getTagger("TrackCountingHighEffBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
		};
		
		LambdaNtupleConsumer<KappaTypes>::Quantities["trailingJetCSV"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(1))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<KappaTypes>::Quantities["trailingJetTCHE"] = [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(1))->getTagger("TrackCountingHighEffBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
		};
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
		bool validJet = ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>::AdditionalCriteria(jet, event, product, settings);
		
		// PU Jet ID
		for (std::map<size_t, std::vector<std::string> >::const_iterator puJetIdByIndex = puJetIdsByIndex.begin();
		     puJetIdByIndex != puJetIdsByIndex.end() && validJet; ++puJetIdByIndex)
		{
			if (puJetIdByIndex->first == product.m_validJets.size())
			{
				validJet = validJet && PassPuJetIds(jet, puJetIdByIndex->second, event.m_taggerMetadata);
			}
		}
		
		for (std::map<std::string, std::vector<std::string> >::const_iterator puJetIdByHltName = puJetIdsByHltName.begin();
		     puJetIdByHltName != puJetIdsByHltName.end() && validJet; ++puJetIdByHltName)
		{
			if (puJetIdByHltName->first == "default")
			{
				validJet = validJet && PassPuJetIds(jet, puJetIdByHltName->second, event.m_taggerMetadata);
			}
			else
			{
				LOG(FATAL) << "HLT name dependent PU Jet is not yet implemented!";
			}
		}
		
		// Jet taggers
		for (std::map<std::string, std::vector<float> >::const_iterator jetTaggerLowerCut = jetTaggerLowerCutsByTaggerName.begin();
		     jetTaggerLowerCut != jetTaggerLowerCutsByTaggerName.end() && validJet; ++jetTaggerLowerCut)
		{
			float maxLowerCut = *std::max_element(jetTaggerLowerCut->second.begin(), jetTaggerLowerCut->second.end());
			validJet = validJet && jet->getTagger(jetTaggerLowerCut->first, event.m_taggerMetadata) > maxLowerCut;
		}
		
		for (std::map<std::string, std::vector<float> >::const_iterator jetTaggerUpperCut = jetTaggerUpperCutsByTaggerName.begin();
		     jetTaggerUpperCut != jetTaggerUpperCutsByTaggerName.end() && validJet; ++jetTaggerUpperCut)
		{
			float minUpperCut = *std::min_element(jetTaggerUpperCut->second.begin(), jetTaggerUpperCut->second.end());
			validJet = validJet && jet->getTagger(jetTaggerUpperCut->first, event.m_taggerMetadata) < minUpperCut;
		}
		
		return validJet;
	}

private:
	std::map<size_t, std::vector<std::string> > puJetIdsByIndex;
	std::map<std::string, std::vector<std::string> > puJetIdsByHltName;
	std::map<std::string, std::vector<float> > jetTaggerLowerCutsByTaggerName;
	std::map<std::string, std::vector<float> > jetTaggerUpperCutsByTaggerName;
	
	bool PassPuJetIds(KDataPFTaggedJet* jet, std::vector<std::string> const& puJetIds, KTaggerMetadata* taggerMetadata) const
	{
		bool validJet = true;
		
		for (std::vector<std::string>::const_iterator puJetId = puJetIds.begin();
		     puJetId != puJetIds.end() && validJet; ++puJetId)
		{
			validJet = validJet && jet->getpuJetID(*puJetId, taggerMetadata);
		}
		
		return validJet;
	}
};


