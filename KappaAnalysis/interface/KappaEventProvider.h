
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
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

	virtual void WireEvent(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
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
		if (! settings.GetGenTaus().empty())
			this->m_event.m_genTaus = this->template SecureFileInterfaceGet<KGenTaus>(settings.GetGenTaus());
		if (! settings.GetTauMetadata().empty())
			this->m_event.m_tauMetadata = this->template SecureFileInterfaceGetMeta<KTauMetadata>(settings.GetTauMetadata());

		// Jets
		if (! settings.GetBasicJets().empty())
			this->m_event.m_basicJets = this->template SecureFileInterfaceGet<KBasicJets>(settings.GetBasicJets());
		if (! settings.GetGenJets().empty())
			this->m_event.m_genJets = this->template SecureFileInterfaceGet<KLVs>(settings.GetGenJets());
		if (! settings.GetTaggedJets().empty())
			this->m_event.m_tjets = this->template SecureFileInterfaceGet<KJets>(settings.GetTaggedJets());
		if (! settings.GetPileupDensity().empty())
			this->m_event.m_pileupDensity = this->template SecureFileInterfaceGet<KPileupDensity>(settings.GetPileupDensity());

		// MET info
		if (! settings.GetMet().empty())
			this->m_event.m_met = this->template SecureFileInterfaceGet<KMET>(settings.GetMet());

		//GenMET info
		if (! settings.GetGenMet().empty())
			this->m_event.m_genMet = this->template SecureFileInterfaceGet<KMET>(settings.GetMet());

		// PF candidates info
		if (! settings.GetPFChargedHadronsPileUp().empty())
			this->m_event.m_pfChargedHadronsPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFChargedHadronsPileUp());
		if (! settings.GetPFChargedHadronsNoPileUp().empty())
			this->m_event.m_pfChargedHadronsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFChargedHadronsNoPileUp());
		if (! settings.GetPFNeutralHadronsNoPileUp().empty())
			this->m_event.m_pfNeutralHadronsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFNeutralHadronsNoPileUp());
		if (! settings.GetPFPhotonsNoPileUp().empty())
			this->m_event.m_pfPhotonsNoPileUp = this->template SecureFileInterfaceGet<KPFCandidates>(settings.GetPFPhotonsNoPileUp());
		
		// triggers
		if (! settings.GetTriggerInfos().empty())
			this->m_event.m_triggerObjectMetadata = this->template SecureFileInterfaceGetMeta<KTriggerObjectMetadata>(settings.GetTriggerInfos(), false);
		if (! settings.GetTriggerObjects().empty())
			this->m_event.m_triggerObjects = this->template SecureFileInterfaceGet<KTriggerObjects>(settings.GetTriggerObjects(), false);
		
		// Generator info
		if (! settings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->template SecureFileInterfaceGet<KGenParticles>(settings.GetGenParticles());
	
		// Vertex info
		if (! settings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->template SecureFileInterfaceGet<KBeamSpot>(settings.GetBeamSpot());
		if (! settings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->template SecureFileInterfaceGet<KVertexSummary>(settings.GetVertexSummary());

		// Track summary
		if (! settings.GetTrackSummary().empty())
			this->m_event.m_trackSummary = this->template SecureFileInterfaceGet<KTrackSummary>(settings.GetTrackSummary());

		// HCAL Noise summary
		if (! settings.GetHCALNoiseSummary().empty())
			this->m_event.m_hcalNoiseSummary = this->template SecureFileInterfaceGet<KHCALNoiseSummary>(settings.GetHCALNoiseSummary());

		// Meta data
		if (! settings.GetEventMetadata().empty())
		{
			if(settings.GetInputIsData())
			{
				this->m_event.m_eventInfo = this->template SecureFileInterfaceGet<KEventInfo>(settings.GetEventMetadata());
			}
			else
			{
				this->m_event.m_genEventInfo = this->template SecureFileInterfaceGet<KGenEventInfo>(settings.GetEventMetadata());
				this->m_event.m_eventInfo = this->m_event.m_genEventInfo;
			}
		}
		
		if (! settings.GetLumiMetadata().empty())
		{
			if(settings.GetInputIsData())
			{
				this->m_event.m_lumiInfo = this->template SecureFileInterfaceGetMeta<KLumiInfo>(settings.GetLumiMetadata());
			}
			else
			{
				this->m_event.m_genLumiMetadata = this->template SecureFileInterfaceGetMeta<KGenLumiInfo>(settings.GetLumiMetadata());
				this->m_event.m_lumiInfo = this->m_event.m_genLumiMetadata;
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
