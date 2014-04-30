
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief GlobalProducer, for valid muons.
   
   Valid muons pass the tightId requirement of the muon POG. In addition there is a loose
   isolation requirement. Two collectons are written into the KappaProduct for valid and
   invalid muons. 

   This Producer needs the MuonID (only "tight" is currently implemented) and the
   Year (2011 or 2012) config tags.
*/

template<class TTypes>
class ValidMuonsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
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

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_muons";
	}

	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		muonID = ToMuonID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(globalSettings.GetMuonID())));
		year = globalSettings.GetYear();
	}

	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		muonID = ToMuonID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetMuonID())));
		year = settings.GetYear();
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}


private:
	MuonID muonID;
	int year;

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product) const
	{
		// Apply muon isolation and MuonID
		for (KDataMuons::iterator muon = event.m_muons->begin();
			 muon != event.m_muons->end(); muon++)
		{
			bool validMuon = true;

			// Muon ID according to Muon POG definitions
			if (muonID == MuonID::TIGHT) {
				if (year == 2012)
					validMuon = validMuon && IsTightMuon2012(&(*muon), event, product);
				else if (year == 2011)
					validMuon = validMuon && IsTightMuon2011(&(*muon), event, product);
				else
					LOG(FATAL) << "Tight muon ID for year " << year << " not yet implemented!";
			}
			else
				LOG(FATAL) << "Muon ID of type " << Utility::ToUnderlyingValue(muonID) << " not yet implemented!";
			
			// check possible analysis-specific criteria
			validMuon = validMuon && AdditionalCriteria(&(*muon), event, product);
			
			if (validMuon)
				product.m_validMuons.push_back(&(*muon));
			else
				product.m_invalidMuons.push_back(&(*muon));
		}
	}
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataMuon* muon, event_type const& event, product_type& product) const
	{
		bool validMuon = true;
		
		/* TODO
		// Own loose cuts on muons and muon isolation
		validMuon = validMuon
					&& muon->p4.Pt() > 12.0
					&& std::abs(muon->p4.Eta()) < 5.0
					&& muon->trackIso03 < 3.0;
		*/
		
		return validMuon;
	}
	
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

