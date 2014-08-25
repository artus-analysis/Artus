
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief Producer for electron four momentum corrections.
   
   (No correction implemented yet.)
*/
template<class TTypes>
class ElectronCorrectionsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "ElectronCorrectionsProducer";
	}
	
	virtual void Init(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// create a copy of all electrons in the event
		product.m_correctedElectrons.clear();
		product.m_correctedElectrons.resize(event.m_electrons->size());
		size_t electronIndex = 0;
		for (KDataElectrons::const_iterator electron = event.m_electrons->begin();
			 electron != event.m_electrons->end(); ++electron)
		{
			product.m_correctedElectrons[electronIndex] = std::shared_ptr<KDataElectron>(new KDataElectron(*electron));
			++electronIndex;
		}
		
		// perform corrections on copied electrons
		for (std::vector<std::shared_ptr<KDataElectron> >::iterator electron = product.m_correctedElectrons.begin();
			 electron != product.m_correctedElectrons.end(); ++electron)
		{
			// No general correction available
		
			// perform possible analysis-specific corrections
			AdditionalCorrections(electron->get(), event, product, settings);
		}
		
		// sort vectors of corrected electrons by pt
		std::sort(product.m_correctedElectrons.begin(), product.m_correctedElectrons.end(),
		          [](std::shared_ptr<KDataElectron> electron1, std::shared_ptr<KDataElectron> electron2) -> bool
		          { return electron1->p4.Pt() > electron2->p4.Pt(); });
	}


protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KDataElectron* electron, event_type const& event,
	                                   product_type& product, setting_type const& settings) const
	{
	}

};

