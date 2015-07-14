
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief GlobalProducer, for valid muons.
   
   Valid muons pass the tightId requirement of the muon POG. In addition there is a loose
   isolation requirement. Two collectons are written into the KappaProduct for valid and
   invalid muons. 

   This Producer needs the following config tags:
   ValidMuonsInput (default: auto)
   Year (2011 and 2012 implemented)
   MuonID (tight, (veto), loose)
   MuonIsoType (pf and detector implemented, type user is intended to be used in derived code)
   MuonIso (tight and loose implemented)
   DirectIso
*/

template<class TTypes>
class ValidMuonsProducer: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, KMuon>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class ValidMuonsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	static ValidMuonsInput ToValidMuonsInput(std::string const& validMuonsInput)
	{
		if (validMuonsInput == "uncorrected") return ValidMuonsInput::UNCORRECTED;
		else if (validMuonsInput == "corrected") return ValidMuonsInput::CORRECTED;
		else return ValidMuonsInput::AUTO;
	}

	enum class MuonID : int
	{
		NONE  = -1,
		TIGHT = 0,
		MEDIUM = 1,
		LOOSE = 2,
		VETO = 3,
		FAKEABLE = 4,
	};
	static MuonID ToMuonID(std::string const& muonID)
	{
		if (muonID == "tight") return MuonID::TIGHT;
		else if (muonID == "medium") return MuonID::MEDIUM;
		else if (muonID == "loose") return MuonID::LOOSE;
		else if (muonID == "veto") return MuonID::VETO;
		else if (muonID == "fakeable") return MuonID::FAKEABLE;
		else return MuonID::NONE;
	}
	
	enum class MuonIsoType : int
	{
		NONE  = -1,
		PF = 0,
		DETECTOR = 1,
		USER = 2,
	};
	static MuonIsoType ToMuonIsoType(std::string const& muonIsoType)
	{
		if (muonIsoType == "pf") return MuonIsoType::PF;
		else if (muonIsoType == "detector") return MuonIsoType::DETECTOR;
		else if (muonIsoType == "user") return MuonIsoType::USER;
		else return MuonIsoType::NONE;
	}
	
	enum class MuonIso : int
	{
		NONE  = -1,
		TIGHT = 0,
		LOOSE = 1,
		FAKEABLE = 2,
	};
	static MuonIso ToMuonIso(std::string const& muonIso)
	{
		if (muonIso == "tight") return MuonIso::TIGHT;
		else if (muonIso == "loose") return MuonIso::LOOSE;
		else if (muonIso == "fakeable") return MuonIso::FAKEABLE;
		else return MuonIso::NONE;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ValidMuonsProducer";
	}
	
	ValidMuonsProducer(std::vector<KMuon*> product_type::*validMuons=&product_type::m_validMuons,
	                   std::vector<KMuon*> product_type::*invalidMuons=&product_type::m_invalidMuons,
	                   std::string (setting_type::*GetMuonID)(void) const=&setting_type::GetMuonID,
	                   std::string (setting_type::*GetMuonIsoType)(void) const=&setting_type::GetMuonIsoType,
	                   std::string (setting_type::*GetMuonIso)(void) const=&setting_type::GetMuonIso,
	                   std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const=&setting_type::GetMuonLowerPtCuts,
	                   std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const=&setting_type::GetMuonUpperAbsEtaCuts) :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, KMuon>(GetLowerPtCuts, GetUpperAbsEtaCuts, validMuons),
		m_validMuonsMember(validMuons),
		m_invalidMuonsMember(invalidMuons),
		GetMuonID(GetMuonID),
		GetMuonIsoType(GetMuonIsoType),
		GetMuonIso(GetMuonIso)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::Init(settings);
		ValidPhysicsObjectTools<TTypes, KMuon>::Init(settings);
		
		validMuonsInput = ToValidMuonsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidMuonsInput())));
		
		muonID = ToMuonID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetMuonID)())));
		muonIsoType = ToMuonIsoType(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetMuonIsoType)())));
		muonIso = ToMuonIso(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetMuonIso)())));
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("nMuons", [](event_type const& event, product_type const& product) {
			return product.m_validMuons.size();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuonPt", [](event_type const& event, product_type const& product) {
			return product.m_validMuons.size() >= 1 ? product.m_validMuons[0]->p4.Pt() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuonEta", [](event_type const& event, product_type const& product) {
			return product.m_validMuons.size() >= 1 ? product.m_validMuons[0]->p4.Eta() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingMuonPt", [](event_type const& event, product_type const& product) {
			return product.m_validMuons.size() >= 2 ? product.m_validMuons[1]->p4.Pt() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trailingMuonEta", [](event_type const& event, product_type const& product) {
			return product.m_validMuons.size() >= 2 ? product.m_validMuons[1]->p4.Eta() : DefaultValues::UndefinedDouble;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuMinusPt", [](event_type const& event, product_type const& product) {
			for (unsigned int i = 0; i < product.m_validMuons.size(); i++)
			{
				if (product.m_validMuons[i]->charge() < 0)
				{
					return product.m_validMuons[i]->p4.Pt();
				}
			}
			return DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuPlusPt", [](event_type const& event, product_type const& product) {
			for (unsigned int i = 0; i < product.m_validMuons.size(); i++)
			{
				if (product.m_validMuons[i]->charge() > 0)
				{
					return product.m_validMuons[i]->p4.Pt();
				}
			}
			return DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuMinusEta", [](event_type const& event, product_type const& product) {
			for (unsigned int i = 0; i < product.m_validMuons.size(); i++)
			{
				if (product.m_validMuons[i]->charge() < 0)
				{
					return product.m_validMuons[i]->p4.Eta();
				}
			}
			return DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingMuPlusEta", [](event_type const& event, product_type const& product) {
			for (unsigned int i = 0; i < product.m_validMuons.size(); i++)
			{
				if (product.m_validMuons[i]->charge() > 0)
				{
					return product.m_validMuons[i]->p4.Eta();
				}
			}
			return DefaultValues::UndefinedFloat;
		});
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_muons);
		
		// select input source
		std::vector<KMuon*> muons;
		if ((validMuonsInput == ValidMuonsInput::AUTO && (product.m_correctedMuons.size() > 0)) || (validMuonsInput == ValidMuonsInput::CORRECTED))
		{
			muons.resize(product.m_correctedMuons.size());
			size_t muonIndex = 0;
			for (std::vector<std::shared_ptr<KMuon> >::iterator muon = product.m_correctedMuons.begin();
			     muon != product.m_correctedMuons.end(); ++muon)
			{
				muons[muonIndex] = muon->get();
				++muonIndex;
			}
		}
		else
		{
			muons.resize(event.m_muons->size());
			size_t muonIndex = 0;
			for (KMuons::iterator muon = event.m_muons->begin(); muon != event.m_muons->end(); ++muon)
			{
				muons[muonIndex] = &(*muon);
				++muonIndex;
			}
		}
		
		// Apply muon isolation and MuonID
		for (std::vector<KMuon*>::iterator muon = muons.begin(); muon != muons.end(); ++muon)
		{
			bool validMuon = true;

			// Muon ID according to Muon POG definitions
			if (muonID == MuonID::TIGHT) {
				if (settings.GetYear() == 2015)
					validMuon = validMuon && IsTightMuon2015(*muon, event, product);
				else if (settings.GetYear() == 2012)
					validMuon = validMuon && IsTightMuon2012(*muon, event, product);
				else if (settings.GetYear() == 2011)
					validMuon = validMuon && IsTightMuon2011(*muon, event, product);
				else
					LOG(FATAL) << "Tight muon ID for year " << settings.GetYear() << " not yet implemented!";
			}
			else if (muonID == MuonID::MEDIUM) {
				if (settings.GetYear() == 2015)
					validMuon = validMuon && IsMediumMuon2015(*muon, event, product);
				else
					LOG(FATAL) << "Medium muon ID for year " << settings.GetYear() << " not yet implemented!";
			}
			else if (muonID == MuonID::LOOSE) {
				if (settings.GetYear() == 2015)
					validMuon = validMuon && IsLooseMuon2015(*muon, event, product);
				else if (settings.GetYear() == 2012)
					validMuon = validMuon && IsLooseMuon2012(*muon, event, product);
				else
					LOG(FATAL) << "Loose muon ID for year " << settings.GetYear() << " not yet implemented!";
			}
			else if (muonID == MuonID::VETO)
			{
				validMuon = validMuon && IsVetoMuon(*muon, event, product);
			}
			else if (muonID == MuonID::FAKEABLE)
			{
				validMuon = validMuon && IsFakeableMuon(*muon, event, product);
			}
			else if (muonID != MuonID::NONE)
			{
				LOG(FATAL) << "Muon ID of type " << Utility::ToUnderlyingValue(muonID) << " not yet implemented!";
			}
			
			// Muon Isolation according to Muon POG definitions (independent of year)
			// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Muon_Isolation
			// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Muon_Isolation_AN1
			if (muonIsoType == MuonIsoType::PF) {
				if (muonIso == MuonIso::TIGHT)
					validMuon = validMuon && ((((*muon)->pfIso() / (*muon)->p4.Pt()) < 0.12) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (muonIso == MuonIso::LOOSE)
					validMuon = validMuon && ((((*muon)->pfIso() / (*muon)->p4.Pt()) < 0.20) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (muonIso == MuonIso::FAKEABLE)
					validMuon = validMuon && IsFakeableMuonIso(*muon, event, product, settings);
				else if (muonIso != MuonIso::NONE)
					LOG(FATAL) << "Muon isolation of type " << Utility::ToUnderlyingValue(muonIso) << " not yet implemented!";
			}
			else if (muonIsoType == MuonIsoType::DETECTOR) {
				if (muonIso == MuonIso::TIGHT)
					validMuon = validMuon && ((((*muon)->trackIso / (*muon)->p4.Pt()) < 0.05) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (muonIso == MuonIso::LOOSE)
					validMuon = validMuon && ((((*muon)->trackIso / (*muon)->p4.Pt()) < 0.10) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (muonIso != MuonIso::NONE)
					LOG(FATAL) << "Muon isolation of type " << Utility::ToUnderlyingValue(muonIso) << " not yet implemented!";
			}
			else if (muonIsoType != MuonIsoType::USER && muonIsoType != MuonIsoType::NONE)
				LOG(FATAL) << "Muon isolation type of type " << Utility::ToUnderlyingValue(muonIsoType) << " not yet implemented!";
			
			// kinematic cuts
			validMuon = validMuon && this->PassKinematicCuts(*muon, event, product);
			
			// check possible analysis-specific criteria
			validMuon = validMuon && AdditionalCriteria(*muon, event, product, settings);
			
			if (validMuon)
			{
				(product.*m_validMuonsMember).push_back(*muon);
			}
			else
			{
				(product.*m_invalidMuonsMember).push_back(*muon);
			}
		}
	}


protected:
	MuonID muonID;
	MuonIsoType muonIsoType;
	MuonIso muonIso;
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KMuon* muon, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validMuon = true;
		return validMuon;
	}


private:
	std::vector<KMuon*> product_type::*m_validMuonsMember;
	std::vector<KMuon*> product_type::*m_invalidMuonsMember;
	std::string (setting_type::*GetMuonID)(void) const;
	std::string (setting_type::*GetMuonIsoType)(void) const;
	std::string (setting_type::*GetMuonIso)(void) const;

	ValidMuonsInput validMuonsInput;
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon_selection
	bool IsTightMuon2011(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->isGlobalMuon()
		       && muon->isPFMuon()
		       && muon->globalTrack.chi2 / muon->globalTrack.nDOF < 10.0
		       && muon->globalTrack.nValidMuonHits > 0
		       && muon->nMatches > 1
		       && std::abs(muon->dxy) < 0.2
		       && muon->track.nValidPixelHits > 0
		       && muon->track.nTrackerLayers() > 8;
	}
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon
	bool IsTightMuon2012(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->isGlobalMuon()
		       && muon->isPFMuon()
		       && muon->globalTrack.chi2 / muon->globalTrack.nDOF < 10.0
		       && muon->globalTrack.nValidMuonHits > 0
		       && muon->nMatches > 1
		       && std::abs(muon->dxy) < 0.2
		       && std::abs(muon->dz) < 0.5
		       && muon->track.nValidPixelHits > 0
		       && muon->track.nTrackerLayers() > 5;
	}
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Loose_Muon
	bool IsLooseMuon2012(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->isPFMuon()
		       && (muon->isGlobalMuon() || muon->isTrackerMuon());
	}

	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Loose_Muon
	bool IsLooseMuon2015(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->idLoose();
	}

	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Medium_Muon
	bool IsMediumMuon2015(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->idMedium();
	}

	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2#Tight_Muon
	bool IsTightMuon2015(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->idTight();
	}

	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorkingSummer2013#Muon_Tau_Final_state
	// should be move to Higgs code
	bool IsVetoMuon(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->isPFMuon()
		       && muon->isGlobalMuon()
		       && muon->isTrackerMuon()
		       && (std::abs(muon->dz) < 0.2);
	}
	
	bool IsFakeableMuon(KMuon* muon, event_type const& event, product_type& product) const
	{
		return muon->isGlobalMuon()
		       && std::abs(muon->dxy) < 0.2;
	}
	
	bool IsFakeableMuonIso(KMuon* muon, event_type const& event, product_type& product, setting_type const& settings) const
	{
		bool validMuon = true;
		
		if (muon->p4.Pt() <= 20.0) {
			validMuon = validMuon &&
				   ((muon->trackIso < 8.0) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				   (muon->ecalIso  < 8.0) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				   (muon->hcalIso  < 8.0) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
		}
		else {
			validMuon = validMuon &&
				   (((muon->trackIso / muon->p4.Pt()) < 0.4) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				   ((muon->ecalIso / muon->p4.Pt()) < 0.4) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				   ((muon->hcalIso / muon->p4.Pt()) < 0.4) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
		}
		
		return validMuon;
	}
};

