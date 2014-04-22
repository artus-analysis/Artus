
#pragma once

#include "Artus/KappaAnalysis/interface/KappaEventBase.h"

/**
   \brief Defines the content of the kappa ntuple.

   Defines the objects which are contained in a kappa ntuple. Members are pointer to the corresponding 
   collections of objects in the input file. This class derives from KapaEventBase, which contains
   pointers to the eventMetadata and genEventMetadata, the minimal content of a kappa ntuple. 
*/

class KappaEvent : public KappaEventBase
{
public:
	KappaEvent() : KappaEventBase() {};

	/// pointer to electron collection
	KDataElectrons* m_electrons = 0;

	/// pointer to muon collection
	KDataMuons* m_muons = 0;

	/// pointer to tau collection
	KDataPFTaus* m_taus = 0;
	KTauDiscriminatorMetadata* m_tauDiscriminatorMetadata = 0;

	/// pointer to jet collection
	KDataPFJets* m_jets = 0;

	/// pointer to tagged jet collection
	KDataPFTaggedJets* m_tjets = 0;

	/// pointer to jet area collection
	KJetArea* m_jetArea = 0;

	/// pointer to MET collection
	KDataPFMET* m_met = 0;

	/// pointer to beamspot collection
	KDataBeamSpot* m_beamSpot = 0;

	/// pointer to primary vertex summary
	KVertexSummary* m_vertexSummary = 0;

	/// pointer to generator particles
	KGenParticles* m_genParticles = 0;

	/// pointer to metadata // TODO: move to Artus/Provider
	KLumiMetadata* m_lumiMetadata = 0;
	KGenLumiMetadata* m_genLumiMetadata = 0;
	KEventMetadata* m_eventMetadata = 0;
	KGenEventMetadata* m_genEventMetadata = 0;
	KFilterMetadata* m_filterMetadata = 0;
	KFilterSummary* m_filterSummary = 0;
};


