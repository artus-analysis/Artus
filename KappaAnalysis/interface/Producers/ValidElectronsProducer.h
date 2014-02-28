
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"


/**
   \brief GlobalProducer, for valid electrons.
   
   ... no comment ...
*/

template<class TTypes>
class ValidElectronsProducer: public GlobalProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual std::string GetProducerId() ARTUS_CPP11_OVERRIDE {
		return "valid_electrons";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		for (KDataElectrons::iterator electron = event.m_electrons->begin();
			 electron != event.m_electrons->end(); electron++)
		{
			bool validElectron = true;
			
			// TODO
			
			if (validElectron)
				product.m_validElectrons.push_back(&(*electron));
			else
				product.m_invalidElectrons.push_back(&(*electron));
		}

		return true;
	}
};

