
#pragma once

#include "Artus/Provider/interface/KappaEventProvider.h"

template<class TTypes>
class KappaLeptonEventProvider: public KappaEventProvider<TTypes> {
public:
	
	typedef typename TTypes::global_setting_type global_setting_type;
	
	KappaLeptonEventProvider(FileInterface2 & fi, InputTypeEnum inpType) :
		KappaEventProvider<TTypes>(fi, inpType)
	{

	};

	virtual void WireEvent(global_setting_type const& globalSettings) {

		// Electrons
	

		// Muons
		this->m_event.m_muons = this->m_fi.template Get<KDataMuons>("muons", true);

		// Taus
	

		// Jets
		this->m_event.m_pfJets = this->m_fi.template Get<KDataPFJets>("AK5PFJets", true);
	
		// MET info
		
	
		// Vertex info
		this->m_event.m_beamSpot = this->m_fi.template Get<KDataBeamSpot>("offlineBeamSpot", true);
		this->m_event.m_vertexSummary = this->m_fi.template Get<KVertexSummary>("goodOfflinePrimaryVerticesSummary", true);

		// Meta data	
		this->m_event.m_lumiMetadata = this->m_fi.template Get<KLumiMetadata>("KLumiMetadata", true);
	};
};
