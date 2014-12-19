
#pragma once


#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Core/interface/EventBase.h"

/**
   \brief Defines the content of the kappa ntuple.

   Defines the objects which are contained in a kappa ntuple. Members are pointer to the corresponding 
   collections of objects in the input file. This class derives from KapaEventBase, which contains
   pointers to the eventMetadata and genEventMetadata, the minimal content of a kappa ntuple. 
*/

class KappaEvent : public EventBase
{
public:
	KappaEvent() : EventBase() {};
	
	size_t m_input = 0;

	/// pointer to electron collection
	KElectrons* m_electrons = 0;
	KElectronMetadata* m_electronMetadata = 0;

	/// pointer to muon collection
	KMuons* m_muons = 0;
	KMuonMetadata* m_muonMetadata = 0;

	/// pointer to tau collection
	KTaus* m_taus = 0;
	KTauMetadata* m_tauMetadata = 0;

	/// pointer to jet collection
	KBasicJets* m_basicJets = 0;
	KLVs* m_genJets = 0;

	/// pointer to tagged jet collection
	KJets* m_tjets = 0;

	/// pointer to jet area collection
	KPileupDensity* m_pileupDensity = 0;

	/// pointer to MET collection
	KMET* m_met = 0;

	/// pointer to GenMET collection
	KBasicMET* m_genMet = 0;

	/// pointers to PF candidates
	KPFCandidates* m_pfChargedHadronsPileUp = 0;
	KPFCandidates* m_pfChargedHadronsNoPileUp = 0;
	KPFCandidates* m_pfNeutralHadronsNoPileUp = 0;
	KPFCandidates* m_pfPhotonsNoPileUp = 0;

	/// pointer to beamspot collection
	KBeamSpot* m_beamSpot = 0;

	/// pointer to primary vertex summary
	KVertexSummary* m_vertexSummary = 0;

	/// pointer to generator particles
	KGenParticles* m_genParticles = 0;

	/// pointer to generator particles
	KGenTaus* m_genTaus = 0;

	/// pointer to trigger infos and objects
	KTriggerObjectMetadata* m_triggerObjectMetadata = 0;
	KTriggerObjects* m_triggerObjects = 0;

	/// pointer to metadata // TODO: move to Artus/Provider
	KEventInfo* m_eventInfo = 0;
	KGenEventInfo* m_genEventInfo = 0;
	KLumiInfo* m_lumiInfo = 0;
	KGenLumiInfo* m_genLumiMetadata = 0;
	KFilterMetadata* m_filterMetadata = 0;
	KFilterSummary* m_filterSummary = 0;
	KJetMetadata* m_jetMetadata = 0;
};


