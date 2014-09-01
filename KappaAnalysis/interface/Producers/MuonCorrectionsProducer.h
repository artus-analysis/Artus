
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief Producer for muon four momentum corrections.
   
   (No correction implemented yet.)
*/
template<class TTypes>
class MuonCorrectionsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "MuonCorrectionsProducer";
	}
	
	virtual void Init(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// create a copy of all muons in the event
		product.m_correctedMuons.clear();
		product.m_correctedMuons.resize(event.m_muons->size());
		size_t muonIndex = 0;
		for (KDataMuons::const_iterator muon = event.m_muons->begin();
			 muon != event.m_muons->end(); ++muon)
		{
			product.m_correctedMuons[muonIndex] = std::shared_ptr<KDataMuon>(new KDataMuon(*muon));
			++muonIndex;
		}
		
		// perform corrections on copied muons
		for (std::vector<std::shared_ptr<KDataMuon> >::iterator muon = product.m_correctedMuons.begin();
			 muon != product.m_correctedMuons.end(); ++muon)
		{
			// No general correction available
		
			// perform possible analysis-specific corrections
			AdditionalCorrections(muon->get(), event, product, settings);
		}
		
		// sort vectors of corrected muons by pt
		std::sort(product.m_correctedMuons.begin(), product.m_correctedMuons.end(),
		          [](std::shared_ptr<KDataMuon> muon1, std::shared_ptr<KDataMuon> muon2) -> bool
		          { return muon1->p4.Pt() > muon2->p4.Pt(); });
	}


protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KDataMuon* muon, event_type const& event,
	                                   product_type& product, setting_type const& settings) const
	{
	}

};

