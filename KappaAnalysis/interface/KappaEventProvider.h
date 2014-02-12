
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/KappaAnalysis/interface/KappaEventProviderBase.h"

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
			this->m_event.m_electrons = this->template SecureFileInterfaceGet<KDataElectrons>(globalSettings.GetElectrons());

		// Muons
		if(! globalSettings.GetMuons().empty())
			this->m_event.m_muons = this->template SecureFileInterfaceGet<KDataMuons>(globalSettings.GetMuons());

		// Taus
		if(! globalSettings.GetTaus().empty())
			this->m_event.m_taus = this->template SecureFileInterfaceGet<KDataPFTaus>(globalSettings.GetTaus());

		// Jets
		if(! globalSettings.GetJets().empty())
			this->m_event.m_jets = this->template SecureFileInterfaceGet<KDataPFJets>(globalSettings.GetJets());
		if(! globalSettings.GetJetArea().empty())
			this->m_event.m_jetArea = this->template SecureFileInterfaceGet<KJetArea>(globalSettings.GetJetArea());
	
		// MET info
		if(! globalSettings.GetMet().empty())
			this->m_event.m_met = this->template SecureFileInterfaceGet<KDataPFMET>(globalSettings.GetMet());
		
		// Generator info
		if(! globalSettings.GetGenParticles().empty())
			this->m_event.m_genParticles = this->template SecureFileInterfaceGet<KGenParticles>(globalSettings.GetGenParticles());
	
		// Vertex info
		if(! globalSettings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->template SecureFileInterfaceGet<KDataBeamSpot>(globalSettings.GetBeamSpot());
		if(! globalSettings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->template SecureFileInterfaceGet<KVertexSummary>(globalSettings.GetVertexSummary());

		// Meta data // TODO: move to KappaEventProviderBase?
		if(! globalSettings.GetLumiMetadata().empty())
			this->m_event.m_lumiMetadata = this->template SecureFileInterfaceGetMeta<KLumiMetadata>(globalSettings.GetLumiMetadata());
		if(! globalSettings.GetGenLumiMetadata().empty())
			this->m_event.m_genLumiMetadata = this->template SecureFileInterfaceGetMeta<KGenLumiMetadata>(globalSettings.GetGenLumiMetadata());
		if(! globalSettings.GetEventMetadata().empty())
			this->m_event.m_eventMetadata = this->template SecureFileInterfaceGet<KEventMetadata>(globalSettings.GetEventMetadata());
		if(! globalSettings.GetGenEventMetadata().empty())
			this->m_event.m_genEventMetadata = this->template SecureFileInterfaceGet<KGenEventMetadata>(globalSettings.GetGenEventMetadata());
		if(! globalSettings.GetFilterMetadata().empty())
			this->m_event.m_filterMetadata = this->template SecureFileInterfaceGetMeta<KFilterMetadata>(globalSettings.GetFilterMetadata()); // TODO: Check
		if(! globalSettings.GetFilterSummary().empty())
			this->m_event.m_filterSummary = this->template SecureFileInterfaceGetMeta<KFilterSummary>(globalSettings.GetFilterSummary()); // TODO: Check
	}


protected:

	template<typename T>
	T* SecureFileInterfaceGet(const std::string &name, const bool check = true, const bool def = false) {
		T* result = this->m_fi.template Get<T>(name, check, def);
		if(result == 0) LOG_FATAL("Requested branch not found!");
		return result;
	}
	
	template<typename T>
	T* SecureFileInterfaceGetMeta(const std::string &name, const bool check = true, const bool def = false) {
		T* result = this->m_fi.template GetMeta<T>(name, check, def);
		if(result == 0) LOG_FATAL("Requested branch not found!");
		return result;
	}

};
