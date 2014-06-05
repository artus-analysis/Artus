
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief GlobalProducer, for valid muons.
   
   Valid muons pass the tightId requirement of the muon POG. In addition there is a loose
   isolation requirement. Two collectons are written into the KappaProduct for valid and
   invalid muons. 

   This Producer needs the following config tags:
   Year (2011 and 2012 implemented)
   MuonID (only "tight" is currently implemented)
   MuonIsoType (pf and detector implemented, type user is intended to be used in derived code)
   MuonIso (tight and loose implemented)
*/

template<class TTypes>
class ValidMuonsProducer: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, KDataMuon>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class MuonID : int
	{
		NONE  = -1,
		TIGHT = 0,
	};
	static MuonID ToMuonID(std::string const& muonID)
	{
		if (muonID == "tight") return MuonID::TIGHT;
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
	};
	static MuonIso ToMuonIso(std::string const& muonIso)
	{
		if (muonIso == "tight") return MuonIso::TIGHT;
		else if (muonIso == "loose") return MuonIso::LOOSE;
		else return MuonIso::NONE;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_muons";
	}
	
	ValidMuonsProducer() :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, KDataMuon>(&product_type::m_validMuons)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::Init(settings);
		
		muonID = ToMuonID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetMuonID())));
		muonIsoType = ToMuonIsoType(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetMuonIsoType())));
		muonIso = ToMuonIso(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetMuonIso())));
		
		this->lowerPtCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                 this->lowerPtCutsByHltName);
		this->upperAbsEtaCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                     this->upperAbsEtaCutsByHltName);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// Apply muon isolation and MuonID
		for (KDataMuons::iterator muon = event.m_muons->begin();
			 muon != event.m_muons->end(); muon++)
		{
			bool validMuon = true;

			// Muon ID according to Muon POG definitions
			if (muonID == MuonID::TIGHT) {
				if (settings.GetYear() == 2012)
					validMuon = validMuon && IsTightMuon2012(&(*muon), event, product);
				else if (settings.GetYear() == 2011)
					validMuon = validMuon && IsTightMuon2011(&(*muon), event, product);
				else
					LOG(FATAL) << "Tight muon ID for year " << settings.GetYear() << " not yet implemented!";
			}
			else if (muonID != MuonID::NONE)
				LOG(FATAL) << "Muon ID of type " << Utility::ToUnderlyingValue(muonID) << " not yet implemented!";
			
			// Muon Isolation according to Mauon POG definitions (independent of year)
			// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Muon_Isolation
			// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Muon_Isolation_AN1
			if (muonIsoType == MuonIsoType::PF) {
				if (muonIso == MuonIso::TIGHT)
					validMuon = validMuon && muon->pfIso04/muon->p4.Pt() < 0.12;
				else if (muonIso == MuonIso::LOOSE)
					validMuon = validMuon && muon->pfIso04/muon->p4.Pt() < 0.20;
				else if (muonIso != MuonIso::NONE)
					LOG(FATAL) << "Muon isolation of type " << Utility::ToUnderlyingValue(muonIso) << " not yet implemented!";
			}
			else if (muonIsoType == MuonIsoType::DETECTOR) {
				if (muonIso == MuonIso::TIGHT)
					validMuon = validMuon && muon->trackIso03/muon->p4.Pt() < 0.05;
				else if (muonIso == MuonIso::LOOSE)
					validMuon = validMuon && muon->trackIso03/muon->p4.Pt() < 0.10;
				else if (muonIso != MuonIso::NONE)
					LOG(FATAL) << "Muon isolation of type " << Utility::ToUnderlyingValue(muonIso) << " not yet implemented!";
			}
			else if (muonIsoType != MuonIsoType::USER && muonIsoType != MuonIsoType::NONE)
				LOG(FATAL) << "Muon isolation type of type " << Utility::ToUnderlyingValue(muonIsoType) << " not yet implemented!";
			
			// kinematic cuts
			validMuon = validMuon && this->PassKinematicCuts(&(*muon), event, product);
			
			// check possible analysis-specific criteria
			validMuon = validMuon && AdditionalCriteria(&(*muon), event, product, settings);
			
			if (validMuon)
				product.m_validMuons.push_back(&(*muon));
			else
				product.m_invalidMuons.push_back(&(*muon));
		}
	}


protected:
	MuonID muonID;
	MuonIsoType muonIsoType;
	MuonIso muonIso;
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataMuon* muon, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validMuon = true;
		return validMuon;
	}


private:
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon_selection
	bool IsTightMuon2011(KDataMuon* muon, event_type const& event, product_type& product) const
	{
		bool validMuon = true;
		
		validMuon = validMuon
					&& muon->isGlobalMuon()
					&& muon->isPFMuon()
					&& muon->globalTrack.chi2 / muon->globalTrack.nDOF < 10.0
					&& muon->globalTrack.nValidMuonHits > 0
					&& muon->nMatches > 1
					&& std::abs(muon->bestTrack.getDxy(&event.m_vertexSummary->pv)) < 0.2
					&& muon->innerTrack.nValidPixelHits > 0
					&& muon->track.nPixelLayers + muon->track.nStripLayers > 8;
		
		return validMuon;
	}
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon
	bool IsTightMuon2012(KDataMuon* muon, event_type const& event, product_type& product) const
	{
		bool validMuon = true;
		
		validMuon = validMuon
					&& muon->isGlobalMuon()
					&& muon->isPFMuon()
					&& muon->globalTrack.chi2 / muon->globalTrack.nDOF < 10.0
					&& muon->globalTrack.nValidMuonHits > 0
					&& muon->nMatches > 1
					&& std::abs(muon->bestTrack.getDxy(&event.m_vertexSummary->pv)) < 0.2
					&& std::abs(muon->bestTrack.getDz(&event.m_vertexSummary->pv)) < 0.5
					&& muon->innerTrack.nValidPixelHits > 0
					&& muon->track.nPixelLayers + muon->track.nStripLayers > 5;
		
		return validMuon;
	}

};

