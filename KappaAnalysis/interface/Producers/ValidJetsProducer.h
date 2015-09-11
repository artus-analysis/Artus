
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

	enum class ValidJetsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	enum class JetIDVersion : int
	{
		ID2010 = 0,  // old run1 version (most run 1 analyses)
		ID2014 = 1,  // new run1 version (run 1 legacy: old version + muon fraction cut)
		             // first run 2 version identical to run 1 legacy version
		ID73X = 3,   // new run 2 version identical to ID2014 but change in cmssw 7.3.x fraction definitions
		ID73Xtemp = 4, // temporary recommendation for first run 2 events due to ID problems in the forward region
		ID73XnoHF = 5, // as temp but invalidate forward jets
		ID2015 = 6,  // new jet ID for run 2 updated on 2015-09-11
	};

	static ValidJetsInput ToValidJetsInput(std::string const& validJetsInput)
	{
		if (validJetsInput == "uncorrected") return ValidJetsInput::UNCORRECTED;
		else if (validJetsInput == "corrected") return ValidJetsInput::CORRECTED;
		else return ValidJetsInput::AUTO;
	}
	static JetIDVersion ToJetIDVersion(std::string const& jetIDVersion)
	{
		if (jetIDVersion == "2010") return JetIDVersion::ID2010;
		else if (jetIDVersion == "2014") return JetIDVersion::ID2014;
		else if (jetIDVersion == "73x") return JetIDVersion::ID73X;
		else if (jetIDVersion == "73xtemp") return JetIDVersion::ID73Xtemp;
		else if (jetIDVersion == "73xnohf") return JetIDVersion::ID73XnoHF;
		else if (jetIDVersion == "2015") return JetIDVersion::ID2015;
		else LOG(FATAL) << "Jet ID version '" << jetIDVersion << "' is not available";
		return JetIDVersion::ID2015;
	}

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

	virtual void Init(KappaSettings const& settings) override
	{
		KappaProducerBase::Init(settings);
		ValidPhysicsObjectTools<KappaTypes, TValidJet>::Init(settings);

		validJetsInput = ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
		jetIDVersion = ToJetIDVersion(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetIDVersion())));
		jetID = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID()));

		noID = (jetID == "none");
		if (jetID == "medium" && jetIDVersion != JetIDVersion::ID2010)
			LOG(WARNING) << "Since 2012, the medium jet ID is not supported officially any longer.";
		if (!noID && (jetIDVersion == JetIDVersion::ID2010 || jetIDVersion == JetIDVersion::ID2014))
			LOG(WARNING) << "This jet ID version is not valid for 73X samples.";

		maxMuFraction = 1.0f;
		maxCEMFraction = 0.99f;
		maxFraction = 1.0f;
		if (jetID == "tight" || jetID == "tightlepveto")
			maxFraction = 0.90f;
		else if (jetID ==  "medium")
			maxFraction = 0.95f;
		else if (jetID ==  "loose")
			maxFraction = 0.99f;
		else if (!noID)
			LOG(FATAL) << "Jet ID of type '" << jetID << "' not implemented!";
		// special rules
		if (jetIDVersion == JetIDVersion::ID2014 ||
		    jetIDVersion == JetIDVersion::ID73X ||
		    jetIDVersion == JetIDVersion::ID73Xtemp ||
		    jetIDVersion == JetIDVersion::ID73XnoHF ||
		    jetIDVersion == JetIDVersion::ID2015 && jetID == "tightlepveto")
		{
			maxMuFraction = 0.8f;
			maxCEMFraction = maxFraction;
		}

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size();
		} );
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets20",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 20.0);
		} );
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets30",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		} );
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets50",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 50.0);
		} );
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets80",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 80.0);
		} );

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPt",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
		} );
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetEta",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
		} );
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPhi",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
		} );

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPt",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
		} );
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetEta",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
		} );
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPhi",[](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
		} );
	}

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
		assert((event.*m_basicJetsMember));

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

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// https://github.com/cms-sw/cmssw/blob/CMSSW_7_5_X/PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h
			// jets, all eta
			validJet = validJet
					   && ((*jet)->neutralHadronFraction < maxFraction)
					   && ((*jet)->photonFraction + (*jet)->hfEMFraction < maxFraction)
					   && ((*jet)->nConstituents > 1)
					   && ((*jet)->muonFraction < maxMuFraction);
			if (jetIDVersion == JetIDVersion::ID2010 || jetIDVersion == JetIDVersion::ID2014)  // CMSSW <7.3.X
				validJet = validJet && ((*jet)->neutralHadronFraction + (*jet)->hfHadronFraction < maxFraction);

					
			// jets, |eta| < 2.4 (tracker)
			if (std::abs((*jet)->p4.eta()) <= 2.4f)
			{
				validJet = validJet
						   && ((*jet)->chargedHadronFraction > 0.0f)
						   && ((*jet)->nCharged > 0)
						   && ((*jet)->electronFraction < maxCEMFraction);  // == CEM
			}
			// for run 2 startup: temporarily no jet ID in forward region
			if (jetIDVersion == JetIDVersion::ID73Xtemp && std::abs((*jet)->p4.eta()) > 3.0f)
				validJet = true;
			if (jetIDVersion == JetIDVersion::ID73XnoHF && std::abs((*jet)->p4.eta()) > 3.0f)
				validJet = false;
			// for run 2: new jet ID in forward region
			if (jetIDVersion == JetIDVersion::ID2015 && std::abs((*jet)->p4.eta()) > 3.0f)
			{
				validJet = ((*jet)->photonFraction + (*jet)->hfEMFraction < 0.90f)
				           && ((*jet)->nConstituents - (*jet)->nCharged > 10);
			}
			// ability to apply no jet ID
			validJet = validJet || noID;

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

	bool noID;
	std::string jetID;
	ValidJetsInput validJetsInput;
	JetIDVersion jetIDVersion;
	float maxFraction;
	float maxMuFraction;
	float maxCEMFraction;
};



/**
   \brief Producer for valid jets (simple PF jets).

   Operates on the vector event.m_basicJets.
*/
class ValidJetsProducer: public ValidJetsProducerBase<KBasicJet, KBasicJet>
{
public:
	ValidJetsProducer();

	virtual std::string GetProducerId() const override;
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
	virtual std::string GetProducerId() const override;
	virtual void Init(KappaSettings const& settings) override;

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


