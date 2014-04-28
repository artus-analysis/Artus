
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief GlobalProducer, for valid muons.
   
   Valid muons pass the tightId requirement of the muon POG. In addition there is a loose 
   isolation requirement. Two collectons are written into the KappaProduct for valid and 
   invalid muons. 
*/

template<class TTypes>
class ValidMuonsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_muons";
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product, globalSettings.GetMuonID(), globalSettings.GetYear());
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product, settings.GetMuonID(), settings.GetYear());
	}


private:

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product,
	                     std::string const& muonID, int const& year) const
	{
		// prepare settings
		std::string tmpMuonID = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(muonID));
	
		// Apply muon isolation and MuonID
		for (KDataMuons::iterator muon = event.m_muons->begin();
			 muon != event.m_muons->end(); muon++)
		{
			bool validMuon = true;

			// Own loose cuts on muons and muon isolation
			validMuon = validMuon
						&& muon->p4.Pt() > 12.0
						&& std::abs(muon->p4.Eta()) < 5.0
						&& muon->trackIso03 < 3.0;

			// Muon ID according to Muon POG definitions
			if (tmpMuonID == "tight") {
				if (year == 2012)
					validMuon = validMuon && IsTightMuon2012(&(*muon), event, product);
				else if (year == 2011)
					validMuon = validMuon && IsTightMuon2011(&(*muon), event, product);
				else
					LOG(FATAL) << "Tight muon ID for year " << year << " not yet implemented!";
			}
			else
				LOG(FATAL) << "Muon ID of type " << muonID << " not yet implemented!";
			
			if (validMuon)
				product.m_validMuons.push_back(&(*muon));
			else
				product.m_invalidMuons.push_back(&(*muon));
		}
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

