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

	long m_input = 0;

	/// pointer to electron collection
	KElectrons* m_electrons = nullptr;
	KElectronMetadata* m_electronMetadata = nullptr;

	/// pointer to muon collection
	KMuons* m_muons = nullptr;
	KMuonMetadata* m_muonMetadata = nullptr;

	/// pointer to tau collection
	KTaus* m_taus = nullptr;
	KTauMetadata* m_tauMetadata = nullptr;
	KGenTaus* m_genTaus = nullptr;
	KGenJets* m_genTauJets = nullptr;

	/// pointer to jet collection
	KBasicJets* m_basicJets = nullptr;
	KGenJets* m_genJets = nullptr;

	/// pointer to tagged jet collection
	KJets* m_tjets = nullptr;

	/// pointer to jet area collection
	KPileupDensity* m_pileupDensity = nullptr;

	/// pointer to MET collection
	KMET* m_met = nullptr;

	/// pointer to Puppi MET collection
	KMET* m_puppiMet = nullptr;

	/// pointer to GenMET collection
	KMET* m_genMet = nullptr;

	/// pointers to PF candidates
	KPFCandidates* m_pfChargedHadronsPileUp = nullptr;
	KPFCandidates* m_pfChargedHadronsNoPileUp = nullptr;
	KPFCandidates* m_pfNeutralHadronsNoPileUp = nullptr;
	KPFCandidates* m_pfPhotonsNoPileUp = nullptr;
	KPFCandidates* m_pfAllChargedParticlesNoPileUp = nullptr;
	KPFCandidates* m_pfAllChargedParticlesPileUp = nullptr;
	KPFCandidates* m_packedPFCandidates = nullptr;

	/// pointer to beamspot collection
	KBeamSpot* m_beamSpot = nullptr;

	/// pointer to primary vertex summary
	KVertexSummary* m_vertexSummary = nullptr;

	/// pointer to the refitted vertices collection
	KRefitVertices* m_refitVertices = nullptr;
	KRefitVertices* m_refitBSVertices = nullptr;

	/// pointer to track summary
	KTrackSummary* m_trackSummary = nullptr;

	/// pointer to HCALNoise summary
	KHCALNoiseSummary* m_hcalNoiseSummary = nullptr;

	/// pointer to generator particles
	KGenParticles* m_genParticles = nullptr;
	KGenParticles* m_lheParticles = nullptr;

	/// pointer to trigger infos and objects
	KTriggerObjectMetadata* m_triggerObjectMetadata = nullptr;
	KTriggerObjects* m_triggerObjects = nullptr;

	/// pointer to metadata // TODO: move to Artus/Provider
	KEventInfo* m_eventInfo = nullptr;
	KGenEventInfo* m_genEventInfo = nullptr;
	KLumiInfo* m_lumiInfo = nullptr;
	KGenLumiInfo* m_genLumiInfo = nullptr;
	KFilterMetadata* m_filterMetadata = nullptr;
	KFilterSummary* m_filterSummary = nullptr;
	KJetMetadata* m_jetMetadata = nullptr;
	KGenEventInfoMetadata *m_genEventInfoMetadata = nullptr;
};


