
#pragma once

#include "Artus/Provider/interface/KappaEvent.h"

class KappaLeptonEvent : public KappaEvent
{
public:
	KappaLeptonEvent() : KappaEvent() {};

	// Electrons
	

	// Muons
	KDataMuons* m_muons = 0;

	// Taus
	

	// Jets
	KDataPFJets* m_pfJets = 0;
	
	// MET info
	
	// Vertex info
	KDataBeamSpot* m_beamSpot = 0;
	KVertexSummary* m_vertexSummary = 0;

	// Meta data
	KLumiMetadata* m_lumiMetadata = 0;
};


