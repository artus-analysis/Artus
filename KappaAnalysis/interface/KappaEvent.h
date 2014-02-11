
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
	KJetArea* m_jetArea = 0;
	
	// MET info
	KDataPFMET* m_pfMet = 0;
	
	// Vertex info
	KDataBeamSpot* m_beamSpot = 0;
	KVertexSummary* m_vertexSummary = 0;
	
	// Generator info
	KGenParticles* m_particles = 0;
	

	// Meta data
	KLumiMetadata* m_lumiMetadata = 0;
	KGenLumiMetadata* m_genLumiMetadata = 0;
	KEventMetadata* m_eventMetadata = 0;
	KGenEventMetadata* m_genEventMetadata = 0;
	KFilterMetadata* m_filterMetadata = 0;
	KFilterSummary* m_filterSummary = 0;
};


