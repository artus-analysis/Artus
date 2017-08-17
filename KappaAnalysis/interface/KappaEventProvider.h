
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
			this->m_event.m_electrons = this->template SecureFileInterfaceGet<KElectrons>(settings.GetElectrons());
		if (! settings.GetElectronMetadata().empty())
            this->m_event.m_electronMetadata = this->template SecureFileInterfaceGetMeta<KElectronMetadata>(settings.GetElectronMetadata());

		// Muons
		if (! settings.GetMuons().empty())
			this->m_event.m_muons = this->template SecureFileInterfaceGet<KMuons>(settings.GetMuons());

		// Taus
		if (! settings.GetTaus().empty())
			this->m_event.m_taus = this->template SecureFileInterfaceGet<KTaus>(settings.GetTaus());
		if (! settings.GetTauMetadata().empty())
			this->m_event.m_tauMetadata = this->template SecureFileInterfaceGetMeta<KTauMetadata>(settings.GetTauMetadata());
		if (! settings.GetGenTaus().empty())
			this->m_event.m_genTaus = this->template SecureFileInterfaceGet<KGenTaus>(settings.GetGenTaus());
		if (! settings.GetGenTauJets().empty())
			this->m_event.m_genTauJets = this->template SecureFileInterfaceGet<KGenJets>(settings.GetGenTauJets());

		// Jets
		if (! settings.GetBasicJets().empty())
			this->m_event.m_basicJets = this->template SecureFileInterfaceGet<KBasicJets>(settings.GetBasicJets());
		if (! settings.GetGenJets().empty()){
			if (settings.GetUseKLVGenJets()) this->m_event.m_genJets = (KGenJets*) this->template SecureFileInterfaceGet<KLVs>(settings.GetGenJets());
			else this->m_event.m_genJets = this->template SecureFileInterfaceGet<KGenJets>(settings.GetGenJets());
			
		}
		if (! settings.GetTaggedJets().empty())
			this->m_event.m_tjets = this->template SecureFileInterfaceGet<KJets>(settings.GetTaggedJets());
		if (! settings.GetPileupDensity().empty())
			this->m_event.m_pileupDensity = this->template SecureFileInterfaceGet<KPileupDensity>(settings.GetPileupDensity());

		// MET info
		if (! settings.GetMet().empty())
			this->m_event.m_met = this->template SecureFileInterfaceGet<KMET>(settings.GetMet(), false);

		if (! settings.GetPuppiMet().empty())
			this->m_event.m_puppiMet = this->template SecureFileInterfaceGet<KMET>(settings.GetPuppiMet(), false);

		//GenMET info
		if (! settings.GetGenMet().empty())
			this->m_event.m_genMet = this->template SecureFileInterfaceGet<KMET>(settings.GetGenMet());

		// PF candidates info
		if (! settings.GetPFChargedHadronsPileUp().empty())
			this->m_event.m_pfChargedHadronsPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFChargedHadronsPileUp());
		if (! settings.GetPFChargedHadronsNoPileUp().empty())
			this->m_event.m_pfChargedHadronsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFChargedHadronsNoPileUp());
		if (! settings.GetPFNeutralHadronsNoPileUp().empty())
			this->m_event.m_pfNeutralHadronsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFNeutralHadronsNoPileUp());
		if (! settings.GetPFPhotonsNoPileUp().empty())
			this->m_event.m_pfPhotonsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFPhotonsNoPileUp());
		if (! settings.GetPFAllChargedParticlesNoPileUp().empty())
			this->m_event.m_pfAllChargedParticlesNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFAllChargedParticlesNoPileUp());
		if (! settings.GetPFAllChargedParticlesPileUp().empty())
			this->m_event.m_pfAllChargedParticlesPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFAllChargedParticlesPileUp());
		if (! settings.GetPackedPFCandidates().empty())
			this->m_event.m_packedPFCandidates = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPackedPFCandidates());
		
		// triggers
		if (! settings.GetTriggerInfos().empty())
			this->m_event.m_triggerObjectMetadata = this->template SecureFileInterfaceGetMeta<KTriggerObjectMetadata>(settings.GetTriggerInfos(), false);
		if (! settings.GetTriggerObjects().empty())
			this->m_event.m_triggerObjects = this->template SecureFileInterfaceGet<KTriggerObjects>(settings.GetTriggerObjects(), false);
		
		// Generator info
		if (! settings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->template SecureFileInterfaceGet<KGenParticles>(settings.GetGenParticles());
		if (! settings.GetLheParticles().empty())
			this->m_event.m_lheParticles = this->template SecureFileInterfaceGet<KLHEParticles>(settings.GetLheParticles(), false);
	
		// Vertex info
		if (! settings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->template SecureFileInterfaceGet<KBeamSpot>(settings.GetBeamSpot());
		if (! settings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->template SecureFileInterfaceGet<KVertexSummary>(settings.GetVertexSummary());
		if (! settings.GetRefitVertices().empty())
			this->m_event.m_refitVertices = this->template SecureFileInterfaceGet<KRefitVertices>(settings.GetRefitVertices());
		if (! settings.GetRefitBSVertices().empty())
			this->m_event.m_refitBSVertices = this->template SecureFileInterfaceGet<KRefitVertices>(settings.GetRefitBSVertices());

		// Track summary
		if (! settings.GetTrackSummary().empty())
			this->m_event.m_trackSummary = this->template SecureFileInterfaceGet<KTrackSummary>(settings.GetTrackSummary());

		// HCAL Noise summary
		if (! settings.GetHCALNoiseSummary().empty())
			this->m_event.m_hcalNoiseSummary = this->template SecureFileInterfaceGet<KHCALNoiseSummary>(settings.GetHCALNoiseSummary());

		// Meta data
		if(settings.GetInputIsData())
		{
			if (! settings.GetEventInfo().empty())
				this->m_event.m_eventInfo = this->template SecureFileInterfaceGet<KEventInfo>(settings.GetEventInfo());
			
			if (! settings.GetLumiInfo().empty())
			{
				this->m_event.m_dataLumiInfo = this->template SecureFileInterfaceGetMeta<KDataLumiInfo>(settings.GetLumiInfo());
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
				this->m_event.m_genEventInfoMetadata = this->template SecureFileInterfaceGetMeta<KGenEventInfoMetadata>(settings.GetGenEventInfoMetadata());
			
			if (! settings.GetEventInfo().empty())
			{
				this->m_event.m_genEventInfo = this->template SecureFileInterfaceGet<KGenEventInfo>(settings.GetEventInfo());
				this->m_event.m_eventInfo = this->m_event.m_genEventInfo;
			}
			
			if (! settings.GetLumiInfo().empty())
			{
				this->m_event.m_genLumiInfo = this->template SecureFileInterfaceGetMeta<KGenLumiInfo>(settings.GetLumiInfo());
				this->m_event.m_lumiInfo = this->m_event.m_genLumiInfo;
			}
			
			if (! settings.GetRunInfo().empty())
			{
				this->m_event.m_genRunInfo = this->template SecureFileInterfaceGetRun<KGenRunInfo>(settings.GetRunInfo());
				this->m_event.m_runInfo = this->m_event.m_genRunInfo;
			}
		}
		
		if (! settings.GetFilterMetadata().empty())
			this->m_event.m_filterMetadata = this->template SecureFileInterfaceGetMeta<KFilterMetadata>(settings.GetFilterMetadata()); // TODO: Check
		if (! settings.GetFilterSummary().empty())
			this->m_event.m_filterSummary = this->template SecureFileInterfaceGetMeta<KFilterSummary>(settings.GetFilterSummary()); // TODO: Check
		if (! settings.GetJetMetadata().empty())
			this->m_event.m_jetMetadata = this->template SecureFileInterfaceGetMeta<KJetMetadata>(settings.GetJetMetadata());

		KappaEventProviderBase<TTypes>::WireEvent(settings);
	}

};
