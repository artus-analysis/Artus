
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

   This is a templated base version. Use the actual versions for KBasicJets or KJets
   at the end of this file.
*/


template<class TJet, class TValidJet>
class ValidJetsProducerBase: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, TValidJet>
{

public:

	ValidJetsProducerBase(std::vector<TJet>* KappaEvent::*jets,
	                      std::vector<std::shared_ptr<TJet> > KappaProduct::*correctJets,
	                      std::vector<TValidJet*> KappaProduct::*validJets) :
		KappaProducerBase(),
		ValidPhysicsObjectTools<KappaTypes, TValidJet>(&KappaSettings::GetJetLowerPtCuts,
		                                               &KappaSettings::GetJetUpperAbsEtaCuts,
		                                               validJets),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctJets)
	{
	}

	void Init(KappaSettings const& settings) override
	{
		KappaProducerBase::Init(settings);
		ValidPhysicsObjectTools<KappaTypes, TValidJet>::Init(settings);

		validJetsInput = KappaEnumTypes::ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
		jetIDVersion = KappaEnumTypes::ToJetIDVersion(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetIDVersion())));
		jetID = KappaEnumTypes::ToJetID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID())));

		if (jetID == KappaEnumTypes::JetID::MEDIUM && jetIDVersion != KappaEnumTypes::JetIDVersion::ID2010)
			LOG(WARNING) << "Since 2012, the medium jet ID is not supported officially any longer.";
		if (jetID != KappaEnumTypes::JetID::NONE && (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2010 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014))
			LOG(WARNING) << "This jet ID version is not valid for 73X samples.";

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size();
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets20",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 20.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets30",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets50",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 50.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets80",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 80.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets20Eta2p4",[this](KappaEvent const& event, KappaProduct const& product) {
            std::vector<TValidJet*> filteredJets;
			for (typename std::vector<TValidJet*>::const_iterator jet = (product.m_validJets).begin();
				 jet != (product.m_validJets).end(); ++jet)
			{
				if ((*jet)->p4.Eta() < 2.4) filteredJets.push_back(new TValidJet(*(*jet)));
			}
			return KappaProduct::GetNJetsAbovePtThreshold(filteredJets, 20.0);
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("thirdJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fourthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fifthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("sixthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.mass() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
		assert((event.*m_basicJetsMember));

		// select input source
		std::vector<TJet*> jets;
		if ((validJetsInput == KappaEnumTypes::ValidJetsInput::AUTO && ((product.*m_correctedJetsMember).size() > 0)) || (validJetsInput == KappaEnumTypes::ValidJetsInput::CORRECTED))
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
			jets.resize((event.*m_basicJetsMember)->size());
			size_t jetIndex = 0;
			for (typename std::vector<TJet>::iterator jet = (event.*m_basicJetsMember)->begin(); jet != (event.*m_basicJetsMember)->end(); ++jet)
			{
				jets[jetIndex] = &(*jet);
				++jetIndex;
			}
		}

		for (typename std::vector<TJet*>::iterator jet = jets.begin(); jet != jets.end(); ++jet)
		{
			bool validJet = true;

			validJet = validJet && passesJetID(*jet, jetIDVersion, jetID);

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
				product.m_validJets.push_back(*jet);
			else
				product.m_invalidJets.push_back(*jet);
		}
	}

	static bool passesJetID(TJet* jet, KappaEnumTypes::JetIDVersion jetIDVersion, KappaEnumTypes::JetID jetID)
	{
		bool validJet = true;

		float maxMuFraction = 1.0f;
		float maxCEMFraction = 0.99f;
		float maxFraction = 1.0f;
		if (jetID == KappaEnumTypes::JetID::TIGHT || jetID == KappaEnumTypes::JetID::TIGHTLEPVETO)
			maxFraction = 0.90f;
		else if (jetID ==  KappaEnumTypes::JetID::MEDIUM)
			maxFraction = 0.95f;
		else if (jetID ==  KappaEnumTypes::JetID::LOOSE || jetID == KappaEnumTypes::JetID::LOOSELEPVETO)
			maxFraction = 0.99f;
		// special rules
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014 ||
			jetIDVersion == KappaEnumTypes::JetIDVersion::ID73X ||
			jetIDVersion == KappaEnumTypes::JetIDVersion::ID73Xtemp ||
			jetIDVersion == KappaEnumTypes::JetIDVersion::ID73XnoHF ||
			(jetIDVersion == KappaEnumTypes::JetIDVersion::ID2015 && (jetID == KappaEnumTypes::JetID::TIGHTLEPVETO || jetID == KappaEnumTypes::JetID::LOOSELEPVETO)) ||
			((jetIDVersion == KappaEnumTypes::JetIDVersion::ID2016 && (jetID == KappaEnumTypes::JetID::TIGHTLEPVETO || jetID == KappaEnumTypes::JetID::LOOSELEPVETO))))
		{
			maxMuFraction = 0.8f;
			maxCEMFraction = maxFraction;
		}

		// JetID
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
		// https://github.com/cms-sw/cmssw/blob/CMSSW_7_5_X/PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h
		// jets, all eta
		validJet = validJet
				   && (jet->neutralHadronFraction < maxFraction)
				   && (jet->photonFraction + jet->hfEMFraction < maxFraction)
				   && (jet->nConstituents > 1)
				   && (jet->muonFraction < maxMuFraction);
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2010 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014)  // CMSSW <7.3.X
			validJet = validJet && (jet->neutralHadronFraction + jet->hfHadronFraction < maxFraction);


		// jets, |eta| < 2.4 (tracker)
		if (std::abs(jet->p4.eta()) <= 2.4f)
		{
			validJet = validJet
					   && (jet->chargedHadronFraction > 0.0f)
					   && (jet->nCharged > 0)
					   && (jet->electronFraction < maxCEMFraction);  // == CEM
		}
		// for run2: new jet ID between 2.7 < |eta| <= 3.0
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2015 && std::abs(jet->p4.eta()) > 2.7f && std::abs(jet->p4.eta()) <= 3.0f)
		{
			validJet = (jet->photonFraction + jet->hfEMFraction < 0.90f)
					   && (jet->nConstituents - jet->nCharged > 2);
		}
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2016 && std::abs(jet->p4.eta()) > 2.7f && std::abs(jet->p4.eta()) <= 3.0f)
		{
			validJet = (jet->photonFraction + jet->hfEMFraction > 0.01f)
						&& (jet->neutralHadronFraction < 0.98f)
						&& (jet->nConstituents - jet->nCharged > 2);
		}
		// for run 2 startup: temporarily no jet ID in forward region
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID73Xtemp && std::abs(jet->p4.eta()) > 3.0f)
			validJet = true;
		if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID73XnoHF && std::abs(jet->p4.eta()) > 3.0f)
			validJet = false;
		// for run 2: new jet ID in forward region
		if ((jetIDVersion == KappaEnumTypes::JetIDVersion::ID2015 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2016) && std::abs(jet->p4.eta()) > 3.0f)
		{
			validJet = (jet->photonFraction + jet->hfEMFraction < 0.90f)
					   && (jet->nConstituents - jet->nCharged > 10);
		}
		// ability to apply no jet ID
		validJet = validJet || (jetID == KappaEnumTypes::JetID::NONE);

		return validJet;
	}


protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
		return true;
	}


private:
	std::vector<TJet>* KappaEvent::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > KappaProduct::*m_correctedJetsMember;

	KappaEnumTypes::ValidJetsInput validJetsInput;
	KappaEnumTypes::JetIDVersion jetIDVersion;
	KappaEnumTypes::JetID jetID;
};



/**
   \brief Producer for valid jets (simple PF jets).

   Operates on the vector event.m_basicJets.
*/
class ValidJetsProducer: public ValidJetsProducerBase<KBasicJet, KBasicJet>
{
public:
	ValidJetsProducer();

	std::string GetProducerId() const override;
};



/**
   \brief Producer for valid jets (tagged PF jets).

   Operates on the vector event.m_tjets.

   Required config tags:
   - PuJetIDs
   - JetTaggerLowerCuts
*/
class ValidTaggedJetsProducer: public ValidJetsProducerBase<KJet, KBasicJet>
{
public:
	ValidTaggedJetsProducer();
	std::string GetProducerId() const override;
	void Init(KappaSettings const& settings) override;

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const;

private:
	std::map<size_t, std::vector<std::string> > puJetIdsByIndex;
	std::map<std::string, std::vector<std::string> > puJetIdsByHltName;
	std::map<std::string, std::vector<float> > jetTaggerLowerCutsByTaggerName;
	std::map<std::string, std::vector<float> > jetTaggerUpperCutsByTaggerName;

	bool PassPuJetIds(KJet* jet, std::vector<std::string> const& puJetIds, KJetMetadata* taggerMetadata) const;
};


