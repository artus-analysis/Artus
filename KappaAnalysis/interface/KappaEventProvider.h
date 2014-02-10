
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
	

		// Muons
		if(! globalSettings.GetMuons().empty())
			this->m_event.m_muons = this->m_fi.template Get<KDataMuons>(globalSettings.GetMuons(), true);

		// Taus
	

		// Jets
		if(! globalSettings.GetJets().empty())
			this->m_event.m_pfJets = this->m_fi.template Get<KDataPFJets>(globalSettings.GetJets(), true);
	
		// MET info
		
	
		// Vertex info
		if(! globalSettings.GetBeamSpot().empty())
			this->m_event.m_beamSpot = this->m_fi.template Get<KDataBeamSpot>(globalSettings.GetBeamSpot(), true);
		
		if(! globalSettings.GetVertexSummary().empty())
			this->m_event.m_vertexSummary = this->m_fi.template Get<KVertexSummary>(globalSettings.GetVertexSummary(), true);

		// Meta data	
		if(! globalSettings.GetLumiMetadata().empty())
			this->m_event.m_lumiMetadata = this->m_fi.template Get<KLumiMetadata>(globalSettings.GetLumiMetadata(), true);
	}
};
