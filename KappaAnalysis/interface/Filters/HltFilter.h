
#pragma once

#include <memory>

#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"


/** 
 *
 *	
 */
template<class TTypes>
class HltFilter: public FilterBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "hlt_filter";
	}
	
	HltFilter() : FilterBase<TTypes>() {};

	virtual bool DoesEventPass(event_type const& event,
			product_type const& product,
            setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if (product.selectedHltName.empty()) {
			// no HLT found
			return false;
		}

		// TODO: Report that we changed the HLT, if we did
		// std::cout << "using trigger " << curName << std::endl;
		return event.m_eventMetadata->hltFired(product.selectedHltName, event.m_lumiMetadata);
	}
};

