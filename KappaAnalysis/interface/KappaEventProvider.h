
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
	
	KappaEventProvider(FileInterface2 & fi, InputTypeEnum inpType) :
		KappaEventProviderBase<TTypes>(fi, inpType)
	{
	}

	virtual void WireEvent(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		
		// Electrons
		if (! settings.GetElectrons().empty())
			this->m_event.m_electrons = this->template SecureFileInterfaceGet<KDataElectrons>(settings.GetElectrons());

		// Muons
		if (! settings.GetMuons().empty())
			this->m_event.m_muons = this->template SecureFileInterfaceGet<KDataMuons>(settings.GetMuons());

		// Taus
		if (! settings.GetTaus().empty())
			this->m_event.m_taus = this->template SecureFileInterfaceGet<KDataPFTaus>(settings.GetTaus());
		if (! settings.GetGenTaus().empty())
			this->m_event.m_genTaus = this->template SecureFileInterfaceGet<KDataGenTaus>(settings.GetGenTaus());
		if (! settings.GetTauDiscriminatorMetadata().empty())
			this->m_event.m_tauDiscriminatorMetadata = this->template SecureFileInterfaceGetMeta<KTauDiscriminatorMetadata>(settings.GetTauDiscriminatorMetadata());

		// Jets
		if (! settings.GetJets().empty())
			this->m_event.m_jets = this->template SecureFileInterfaceGet<KDataPFJets>(settings.GetJets());
		if (! settings.GetGenJets().empty())
			this->m_event.m_genJets = this->template SecureFileInterfaceGet<KDataLVs>(settings.GetGenJets());
		if (! settings.GetTaggedJets().empty())
			this->m_event.m_tjets = this->template SecureFileInterfaceGet<KDataPFTaggedJets>(settings.GetTaggedJets());
		if (! settings.GetJetArea().empty())
			this->m_event.m_jetArea = this->template SecureFileInterfaceGet<KJetArea>(settings.GetJetArea());
	
		// MET info
		if (! settings.GetMet().empty())
			this->m_event.m_met = this->template SecureFileInterfaceGet<KDataPFMET>(settings.GetMet());

		//GenMET info
		if (! settings.GetGenMet().empty())
			this->m_event.m_genMet = this->template SecureFileInterfaceGet<KDataPFMET>(settings.GetMet());

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
			this->m_event.m_triggerInfos = this->template SecureFileInterfaceGetMeta<KTriggerInfos>(settings.GetTriggerInfos(), false);
		if (! settings.GetTriggerObjects().empty())
			this->m_event.m_triggerObjects = this->template SecureFileInterfaceGet<KTriggerObjects>(settings.GetTriggerObjects(), false);
		
		// Generator info
		if (! settings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->template SecureFileInterfaceGet<KGenParticles>(settings.GetGenParticles());
	
		// Vertex info
		if (! settings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->template SecureFileInterfaceGet<KDataBeamSpot>(settings.GetBeamSpot());
		if (! settings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->template SecureFileInterfaceGet<KVertexSummary>(settings.GetVertexSummary());

		// Meta data
		if (! settings.GetEventMetadata().empty())
		{
			if(settings.GetInputIsData())
			{
				this->m_event.m_eventMetadata = this->template SecureFileInterfaceGet<KEventMetadata>(settings.GetEventMetadata());
			}
			else
			{
				this->m_event.m_genEventMetadata = this->template SecureFileInterfaceGet<KGenEventMetadata>(settings.GetEventMetadata());
				this->m_event.m_eventMetadata = this->m_event.m_genEventMetadata;
			}
		}
		
		if (! settings.GetLumiMetadata().empty())
		{
			if(settings.GetInputIsData())
			{
				this->m_event.m_lumiMetadata = this->template SecureFileInterfaceGetMeta<KLumiMetadata>(settings.GetLumiMetadata());
			}
			else
			{
				this->m_event.m_genLumiMetadata = this->template SecureFileInterfaceGetMeta<KGenLumiMetadata>(settings.GetLumiMetadata());
				this->m_event.m_lumiMetadata = this->m_event.m_genLumiMetadata;
			}
		}
		if (! settings.GetFilterMetadata().empty())
			this->m_event.m_filterMetadata = this->template SecureFileInterfaceGetMeta<KFilterMetadata>(settings.GetFilterMetadata()); // TODO: Check
		if (! settings.GetFilterSummary().empty())
			this->m_event.m_filterSummary = this->template SecureFileInterfaceGetMeta<KFilterSummary>(settings.GetFilterSummary()); // TODO: Check
		if (! settings.GetTaggerMetadata().empty())
			this->m_event.m_taggerMetadata = this->template SecureFileInterfaceGetMeta<KTaggerMetadata>(settings.GetTaggerMetadata());
		
		KappaEventProviderBase<TTypes>::WireEvent(settings);
	}

};
