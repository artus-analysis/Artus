
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"


/** Select only valid taus.

    The taus are valid if the pass these criteria
    1. ...
*/
template<class TTypes>
class ValidTausProducer: public GlobalProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual std::string GetProducerId() {
		return "valid_taus";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		for (KDataPFTaus::iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); tau++)
		{
			bool good_tau = true;
			
			// TODO
			
			if (good_tau)
				product.m_validTaus.push_back(&(*tau));
			else
				product.m_invalidTaus.push_back(&(*tau));
		}

		return true;
	}
};

