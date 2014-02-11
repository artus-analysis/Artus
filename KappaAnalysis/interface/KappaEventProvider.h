
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Provider/interface/KappaEventProviderBase.h"

template<class TTypes>
class KappaEventProvider: public KappaEventProviderBase<TTypes> {
public:
	
	typedef typename TTypes::global_setting_type global_setting_type;
	
	KappaEventProvider(FileInterface2 & fi, InputTypeEnum inpType) :
		KappaEventProviderBase<TTypes>(fi, inpType)
	{

	}

	virtual void WireEvent(global_setting_type const& globalSettings) {
		
		// Electrons
		if(! globalSettings.GetElectrons().empty())
			this->m_event.m_electrons = this->m_fi.template Get<KDataElectron>(globalSettings.GetElectrons(), true);

		// Muons
		if(! globalSettings.GetMuons().empty())
			this->m_event.m_muons = this->m_fi.template Get<KDataMuons>(globalSettings.GetMuons(), true);

		// Taus
		if(! globalSettings.GetTaus().empty())
			this->m_event.m_taus = this->m_fi.template Get<KDataTaus>(globalSettings.GetTaus(), true);

		// Jets
		if(! globalSettings.GetJets().empty())
			this->m_event.m_pfJets = this->m_fi.template Get<KDataPFJets>(globalSettings.GetJets(), true);
		if(! globalSettings.GetJetArea().empty())
			this->m_event.m_jetArea = this->m_fi.template Get<KJetArea>(globalSettings.GetJetArea(), true);
	
		// MET info
		if(! globalSettings.GetMet().empty())
			this->m_event.m_pfMet = this->m_fi.template Get<KDataPFMET>(globalSettings.GetMet(), true);
		
		// Generator info
		if(! globalSettings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->m_fi.template Get<KGenParticles>(globalSettings.GetGenParticles(), true);
	
		// Vertex info
		if(! globalSettings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->m_fi.template Get<KDataBeamSpot>(globalSettings.GetBeamSpot(), true);
		if(! globalSettings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->m_fi.template Get<KVertexSummary>(globalSettings.GetVertexSummary(), true);

		// Meta data // TODO: move to Artus/Provider
		if(! globalSettings.GetLumiMetadata().empty())
			this->m_event.m_lumiMetadata = this->m_fi.template GetMeta<KLumiMetadata>(globalSettings.GetLumiMetadata(), true);
		if(! globalSettings.GetGenLumiMetadata().empty())
			this->m_event.m_genLumiMetadata = this->m_fi.template GetMeta<KGenLumiMetadata>(globalSettings.GetGenLumiMetadata(), true);
		if(! globalSettings.GetEventMetadata().empty())
			this->m_event.m_eventMetadata = this->m_fi.template Get<KEventMetadata>(globalSettings.GetEventMetadata(), true);
		if(! globalSettings.GetGenEventMetadata().empty())
			this->m_event.m_genEventMetadata = this->m_fi.template Get<KGenEventMetadata>(globalSettings.GetGenEventMetadata(), true);
		if(! globalSettings.GetFilterMetadata().empty())
			this->m_event.m_filterMetadata = this->m_fi.template GetMeta<KFilterMetadata>(globalSettings.GetFilterMetadata(), true); // TODO: Check
		if(! globalSettings.GetFilterSummary().empty())
			this->m_event.m_filterSummary = this->m_fi.template GetMeta<KFilterSummary>(globalSettings.GetFilterSummary(), true); // TODO: Check
	}
};
