
#pragma once

#include "Artus/Provider/interface/KappaEventBase.h"

class KappaEvent : public KappaEventBase
{
public:
	KappaEvent() : KappaEventBase() {};

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


