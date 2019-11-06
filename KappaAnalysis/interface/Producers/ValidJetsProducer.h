
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"

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

	ValidJetsProducerBase(std::vector<TJet>* KappaTypes::event_type::*jets,
	                      std::vector<std::shared_ptr<TJet> > KappaTypes::product_type::*correctJets,
	                      std::vector<TValidJet*> KappaTypes::product_type::*validJets) :
		KappaProducerBase(),
		ValidPhysicsObjectTools<KappaTypes, TValidJet>(&KappaTypes::setting_type::GetJetLowerPtCuts,
		                                               &KappaTypes::setting_type::GetJetUpperAbsEtaCuts,
		                                               validJets),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctJets)
	{
	}

	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override
	{
		KappaProducerBase::Init(settings, metadata);
		ValidPhysicsObjectTools<KappaTypes, TValidJet>::Init(settings);

		validJetsInput = KappaEnumTypes::ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
		jetIDVersion = KappaEnumTypes::ToJetIDVersion(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetIDVersion())));
		jetID = KappaEnumTypes::ToJetID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID())));

		if (jetID == KappaEnumTypes::JetID::MEDIUM && jetIDVersion != KappaEnumTypes::JetIDVersion::ID2010)
			LOG(WARNING) << "Since 2012, the medium jet ID is not supported officially any longer.";
		if (jetID != KappaEnumTypes::JetID::NONE && (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2010 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014))
			LOG(WARNING) << "This jet ID version is not valid for 73X samples.";

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size();
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets20",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validJets, 20.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets30",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets50",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validJets, 50.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets80",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return KappaTypes::product_type::GetNJetsAbovePtThreshold(product.m_validJets, 80.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nJets20Eta2p4",[this](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
            std::vector<TValidJet*> filteredJets;
			for (typename std::vector<TValidJet*>::const_iterator jet = (product.m_validJets).begin();
				 jet != (product.m_validJets).end(); ++jet)
			{
				if ((*jet)->p4.Eta() < 2.4) filteredJets.push_back(new TValidJet(*(*jet)));
			}
			return KappaTypes::product_type::GetNJetsAbovePtThreshold(filteredJets, 20.0);
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "leadingJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "leadingJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "leadingJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "leadingJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "leadingJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "trailingJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "trailingJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "trailingJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "trailingJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "trailingJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "thirdJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "thirdJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "thirdJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "thirdJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "thirdJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "fourthJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fourthJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fourthJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fourthJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fourthJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "fifthJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fifthJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fifthJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fifthJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "fifthJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, "sixthJetLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "sixthJetPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "sixthJetEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "sixthJetPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "sixthJetMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.mass() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
	             KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const override
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
			validJet = validJet && AdditionalCriteria(*jet, event, product, settings, metadata);

			if (validJet)
				product.m_validJets.push_back(*jet);
			else
				product.m_invalidJets.push_back(*jet);
		}
	}

	static bool passesJetID(TJet* jet, KappaEnumTypes::JetIDVersion jetIDVersion, KappaEnumTypes::JetID jetID)
	{
		bool validJet = true;
		if (jetIDVersion != KappaEnumTypes::JetIDVersion::ID2017)
		{
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
			// outdated
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// https://github.com/cms-sw/cmssw/blob/CMSSW_7_5_X/PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h
			// jets, all eta
			if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2010 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014)  // CMSSW <7.3.X
				validJet = validJet && (jet->neutralHadronFraction + jet->hfHadronFraction < maxFraction);
		
			if (std::abs(jet->p4.eta()) <= 2.7f)
			{
			validJet = validJet
					   && (jet->neutralHadronFraction < maxFraction)
					   && (jet->photonFraction + jet->hfEMFraction < maxFraction)
					   && (jet->nConstituents > 1)
					   && (jet->muonFraction < maxMuFraction);
					   
				// jets, |eta| < 2.4 (tracker) then additional criteria need to be applied		   
				if (std::abs(jet->p4.eta()) <= 2.4f)
				{
					validJet = validJet
						   && (jet->chargedHadronFraction > 0.0f)
						   && (jet->nCharged > 0)
						   && (jet->electronFraction < maxCEMFraction);  // == CEM
				}
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
		}
		else if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2017)  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
		{
			//only implemented the ak4chs tight
			//Veto ECAL noise jets
			if( (std::abs(jet->p4.eta()) >= 2.65) && (std::abs(jet->p4.eta())) <= 3.139f && (jet->p4.Pt()<50.f))
			{
				validJet = false;
			}
			// criteria |eta|<2.7
			else if (std::abs(jet->p4.eta()) <= 2.7f)
			{
				float MaxNeutralHadronFraction = 0.90f; //smaller
				float MaxNeutralEMFraction = 0.90f; //smaller
				int MinNumberOfConstituens = 1; //larger

				validJet = validJet
						&& (jet->neutralHadronFraction < MaxNeutralHadronFraction)
						&& ((jet->photonFraction + jet->hfEMFraction) < MaxNeutralEMFraction)
						&& (jet->nConstituents > MinNumberOfConstituens);
				//extra criteria for |eta|<2.4
				if (std::abs(jet->p4.eta()) <= 2.4f)
				{
					float MinChargedHadronFraction = 0.f;
					int MinChargedMultiplicity = 0;

					validJet = validJet
							&& (jet->chargedHadronFraction > MinChargedHadronFraction)
							&& (jet->nCharged > MinChargedMultiplicity);
				}
			}
			//criteria 2.7<|eta|<=3.0
			else if ((std::abs(jet->p4.eta()) > 2.7f) && (std::abs(jet->p4.eta()) <= 3.0f))
			{
				float MaxNeutralEMFraction = 0.99f;
				float MinNeutralEMFraction = 0.02f;
				int MinNumberOfNeutralParticles = 2;

				validJet = validJet
						&& ((jet->photonFraction + jet->hfEMFraction) < MaxNeutralEMFraction)
						&& ((jet->photonFraction + jet->hfEMFraction) > MinNeutralEMFraction)
						&& ((jet->nConstituents - jet->nCharged) > MinNumberOfNeutralParticles);
			}
			//criteria |eta|>3.0
			else if (std::abs(jet->p4.eta()) > 3.0f)
			{
				float MaxNeutralEMFraction = 0.90f;
				float MinNeutralHadronFraction = 0.02f;
				int MinNumberOfNeutralParticles = 10;

				validJet = validJet
						&& ((jet->photonFraction + jet->hfEMFraction) < MaxNeutralEMFraction)
						&& (jet->neutralHadronFraction > MinNeutralHadronFraction)
						&& ((jet->nConstituents - jet->nCharged) > MinNumberOfNeutralParticles);
			}
		}
		// ability to apply no jet ID
		validJet = validJet || (jetID == KappaEnumTypes::JetID::NONE);

		return validJet;
	}

	static bool AdditionalCriteriaStatic(TJet* jet, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                     KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
	{
		return true;
	}

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const
	{
		return ValidJetsProducerBase<TJet, TValidJet>::AdditionalCriteriaStatic(jet, event, product, settings, metadata);
	}


private:
	std::vector<TJet>* KappaTypes::event_type::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > KappaTypes::product_type::*m_correctedJetsMember;

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
	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;
	
	static bool AdditionalCriteriaStatic(KJet* jet,
	                                     std::map<size_t, std::vector<std::string> > const& puJetIdsByIndex,
	                                     std::map<std::string, std::vector<std::string> > const& puJetIdsByHltName,
	                                     std::map<std::string, std::vector<float> > const& jetTaggerLowerCutsByTaggerName,
	                                     std::map<std::string, std::vector<float> > const& jetTaggerUpperCutsByTaggerName,
	                                     KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                     KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata);

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KJet* jet, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const;

	std::map<size_t, std::vector<std::string> > puJetIdsByIndex;
	std::map<std::string, std::vector<std::string> > puJetIdsByHltName;
	std::map<std::string, std::vector<float> > jetTaggerLowerCutsByTaggerName;
	std::map<std::string, std::vector<float> > jetTaggerUpperCutsByTaggerName;

private:
	static bool PassPuJetIds(KJet* jet, std::vector<std::string> const& puJetIds, KJetMetadata* taggerMetadata);
};


