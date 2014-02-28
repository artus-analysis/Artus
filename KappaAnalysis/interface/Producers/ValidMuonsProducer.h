
#pragma once

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

	virtual std::string GetProducerId() ARTUS_CPP11_OVERRIDE {
		return "valid_muons";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
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

			// Tight MuonID 2012
			// [twiki](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon)
			// The comments describe, how CMSSW treats the recoMu.
			/// version of MuonID
			bool is2011 = false; //globalSettings.Global()->GetMuonID2011();
			validMuon = validMuon
						&& muon->isGlobalMuon()
						// use PF muons
						&& muon->isPFMuon()
						// normalizedChi2
						&& muon->globalTrack.chi2 / muon->globalTrack.nDOF < 10.
						// hitPattern().numberOfValidMuonHits
						&& muon->globalTrack.nValidMuonHits > 0
						// numberOfMatchedStations
						//&& muon->nMatches > 1 // TODO
						// fabs(muonBestTrack()->dxy(vertex->position))
						&& std::abs(muon->bestTrack.getDxy(&event.m_vertexSummary->pv)) < 0.2
						// fabs(muonBestTrack()->dz(vertex->position)) // not in 2011
						&& std::abs(muon->bestTrack.getDz(&event.m_vertexSummary->pv)) < 0.5 + 99999. * is2011
						// hitPattern().numberOfValidPixelHits()
						&& muon->innerTrack.nValidPixelHits > 0
						// hitPattern().trackerLayersWithMeasurement() // 8 in 2011
						&& muon->track.nPixelLayers + muon->track.nStripLayers > 5 + 3 * is2011;
			
			if (validMuon)
				product.m_validMuons.push_back(&(*muon));
			else
				product.m_invalidMuons.push_back(&(*muon));
		}

		return true;
	}

	// empty to serve as a pure global producer
	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
	}
};

