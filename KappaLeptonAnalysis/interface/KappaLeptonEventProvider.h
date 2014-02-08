
#pragma once

#include "Artus/Provider/interface/KappaEventProvider.h"

template<class TEventType>
class KappaLeptonEventProvider: public KappaEventProvider<TEventType> {
public:
	KappaLeptonEventProvider(FileInterface2 & fi, InputTypeEnum inpType) :
		KappaEventProvider<TEventType>(fi, inpType)
	{

		WireEvent();
	};

	virtual void WireEvent() {

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
