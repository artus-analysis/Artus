
#pragma once

#include "Artus/KappaAnalysis/interface/KappaEventProviderBase.h"

/**
   \brief class to connect the analysis specific event content to the pipelines.

   Defines the basic functionality expected by PipelineRunner and implements the WireEvent function. 
   This function needs to be called after the KappaEventProvider is instantiated in the main 
   executable.
*/

template<class TTypes>
class KappaEventProvider: public KappaEventProviderBase<TTypes> {
public:

	typedef typename TTypes::setting_type setting_type;

	KappaEventProvider(FileInterface2 & fi, InputTypeEnum inpType, bool batchMode=false) :
		KappaEventProviderBase<TTypes>(fi, inpType, batchMode)
	{
	}

	void WireEvent(setting_type const& settings) override {
		// Electrons
		if (! settings.GetElectrons().empty())
			this->m_event.m_electrons = this->template SecureFileInterfaceGetEvent<KElectrons>(settings.GetElectrons());
		if (! settings.GetElectronMetadata().empty())
            this->m_event.m_electronMetadata = this->template SecureFileInterfaceGetLumi<KElectronMetadata>(settings.GetElectronMetadata());

		// Muons
		if (! settings.GetMuons().empty())
			this->m_event.m_muons = this->template SecureFileInterfaceGetEvent<KMuons>(settings.GetMuons());

		// Taus
		if (! settings.GetTaus().empty())
			this->m_event.m_taus = this->template SecureFileInterfaceGetEvent<KTaus>(settings.GetTaus());
		if (! settings.GetTauMetadata().empty())
			this->m_event.m_tauMetadata = this->template SecureFileInterfaceGetLumi<KTauMetadata>(settings.GetTauMetadata());
		if (! settings.GetGenTaus().empty())
			this->m_event.m_genTaus = this->template SecureFileInterfaceGetEvent<KGenTaus>(settings.GetGenTaus());
		if (! settings.GetGenTauJets().empty())
			this->m_event.m_genTauJets = this->template SecureFileInterfaceGetEvent<KGenJets>(settings.GetGenTauJets());

		// Jets
		if (! settings.GetBasicJets().empty())
			this->m_event.m_basicJets = this->template SecureFileInterfaceGetEvent<KBasicJets>(settings.GetBasicJets());
		if (! settings.GetGenJets().empty()){
			if (settings.GetUseKLVGenJets()) this->m_event.m_genJets = (KGenJets*) this->template SecureFileInterfaceGetEvent<KLVs>(settings.GetGenJets());
			else this->m_event.m_genJets = this->template SecureFileInterfaceGetEvent<KGenJets>(settings.GetGenJets());
			
		}
		if (! settings.GetTaggedJets().empty())
			this->m_event.m_tjets = this->template SecureFileInterfaceGetEvent<KJets>(settings.GetTaggedJets());
		if (! settings.GetPileupDensity().empty())
			this->m_event.m_pileupDensity = this->template SecureFileInterfaceGetEvent<KPileupDensity>(settings.GetPileupDensity());

		// MET info
		if (! settings.GetMet().empty())
			this->m_event.m_met = this->template SecureFileInterfaceGetEvent<KMET>(settings.GetMet(), false);

		if (! settings.GetPuppiMet().empty())
			this->m_event.m_puppiMet = this->template SecureFileInterfaceGetEvent<KMET>(settings.GetPuppiMet(), false);

		//GenMET info
		if (! settings.GetGenMet().empty())
			this->m_event.m_genMet = this->template SecureFileInterfaceGetEvent<KMET>(settings.GetGenMet());

		// PF candidates info
		if (! settings.GetPFChargedHadronsPileUp().empty())
			this->m_event.m_pfChargedHadronsPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFChargedHadronsPileUp());
		if (! settings.GetPFChargedHadronsNoPileUp().empty())
			this->m_event.m_pfChargedHadronsNoPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFChargedHadronsNoPileUp());
		if (! settings.GetPFNeutralHadronsNoPileUp().empty())
			this->m_event.m_pfNeutralHadronsNoPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFNeutralHadronsNoPileUp());
		if (! settings.GetPFPhotonsNoPileUp().empty())
			this->m_event.m_pfPhotonsNoPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFPhotonsNoPileUp());
		if (! settings.GetPFAllChargedParticlesNoPileUp().empty())
			this->m_event.m_pfAllChargedParticlesNoPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFAllChargedParticlesNoPileUp());
		if (! settings.GetPFAllChargedParticlesPileUp().empty())
			this->m_event.m_pfAllChargedParticlesPileUp = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPFAllChargedParticlesPileUp());
		if (! settings.GetPackedPFCandidates().empty())
			this->m_event.m_packedPFCandidates = this->template SecureFileInterfaceGetEvent<KPFCandidates>(settings.GetPackedPFCandidates());
		
		// triggers
		if (! settings.GetTriggerInfos().empty())
			this->m_event.m_triggerObjectMetadata = this->template SecureFileInterfaceGetLumi<KTriggerObjectMetadata>(settings.GetTriggerInfos(), false);
		if (! settings.GetTriggerObjects().empty())
			this->m_event.m_triggerObjects = this->template SecureFileInterfaceGetEvent<KTriggerObjects>(settings.GetTriggerObjects(), false);
		
		// Generator info
		if (! settings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->template SecureFileInterfaceGetEvent<KGenParticles>(settings.GetGenParticles());
		if (! settings.GetLheParticles().empty())
			this->m_event.m_lheParticles = this->template SecureFileInterfaceGetEvent<KLHEParticles>(settings.GetLheParticles(), false);
	
		// Vertex info
		if (! settings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->template SecureFileInterfaceGetEvent<KBeamSpot>(settings.GetBeamSpot());
		if (! settings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->template SecureFileInterfaceGetEvent<KVertexSummary>(settings.GetVertexSummary());
		if (! settings.GetRefitVertices().empty())
			this->m_event.m_refitVertices = this->template SecureFileInterfaceGetEvent<KRefitVertices>(settings.GetRefitVertices());
		if (! settings.GetRefitBSVertices().empty())
			this->m_event.m_refitBSVertices = this->template SecureFileInterfaceGetEvent<KRefitVertices>(settings.GetRefitBSVertices());

		// Track summary
		if (! settings.GetTrackSummary().empty())
			this->m_event.m_trackSummary = this->template SecureFileInterfaceGetEvent<KTrackSummary>(settings.GetTrackSummary());

		// HCAL Noise summary
		if (! settings.GetHCALNoiseSummary().empty())
			this->m_event.m_hcalNoiseSummary = this->template SecureFileInterfaceGetEvent<KHCALNoiseSummary>(settings.GetHCALNoiseSummary());

		// Meta data
		if(settings.GetInputIsData())
		{
			if (! settings.GetEventInfo().empty())
				this->m_event.m_eventInfo = this->template SecureFileInterfaceGetEvent<KEventInfo>(settings.GetEventInfo());
			
			if (! settings.GetLumiInfo().empty())
			{
				this->m_event.m_dataLumiInfo = this->template SecureFileInterfaceGetLumi<KDataLumiInfo>(settings.GetLumiInfo());
				this->m_event.m_lumiInfo = this->m_event.m_dataLumiInfo;
			}
			
			if (! settings.GetRunInfo().empty())
			{
				this->m_event.m_dataRunInfo = this->template SecureFileInterfaceGetRun<KDataRunInfo>(settings.GetRunInfo());
				this->m_event.m_runInfo = this->m_event.m_dataRunInfo;
			}
		}
		else
		{
			if (! settings.GetGenEventInfoMetadata().empty())
				this->m_event.m_genEventInfoMetadata = this->template SecureFileInterfaceGetLumi<KGenEventInfoMetadata>(settings.GetGenEventInfoMetadata());
			
			if (! settings.GetEventInfo().empty())
			{
				this->m_event.m_genEventInfo = this->template SecureFileInterfaceGetEvent<KGenEventInfo>(settings.GetEventInfo());
				this->m_event.m_eventInfo = this->m_event.m_genEventInfo;
			}
			
			if (! settings.GetLumiInfo().empty())
			{
				this->m_event.m_genLumiInfo = this->template SecureFileInterfaceGetLumi<KGenLumiInfo>(settings.GetLumiInfo());
				this->m_event.m_lumiInfo = this->m_event.m_genLumiInfo;
			}
			
			if (! settings.GetRunInfo().empty())
			{
				this->m_event.m_genRunInfo = this->template SecureFileInterfaceGetRun<KGenRunInfo>(settings.GetRunInfo());
				this->m_event.m_runInfo = this->m_event.m_genRunInfo;
			}
		}
		
		if (! settings.GetFilterMetadata().empty())
			this->m_event.m_filterMetadata = this->template SecureFileInterfaceGetLumi<KFilterMetadata>(settings.GetFilterMetadata());
		if (! settings.GetFilterSummary().empty())
			this->m_event.m_filterSummary = this->template SecureFileInterfaceGetLumi<KFilterSummary>(settings.GetFilterSummary());
		if (! settings.GetJetMetadata().empty())
			this->m_event.m_jetMetadata = this->template SecureFileInterfaceGetLumi<KJetMetadata>(settings.GetJetMetadata());

		KappaEventProviderBase<TTypes>::WireEvent(settings);
	}

};
