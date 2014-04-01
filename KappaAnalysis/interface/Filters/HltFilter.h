
#pragma once

#include <memory>

#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"


/** Filter events for that a previously selected HLT trigger has fired.
 *
 *  The selected HLT trigger has to be defined in the product by the HltProducer.
 *  Therefore this filter cannot meaningfully run as a global pre-filter
 *  which gets an empty product.
 */
template<class TTypes>
class HltFilter: public FilterBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "hlt_filter";
	}
	
	HltFilter() : FilterBase<TTypes>() {};

	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE
	{
		LOG(FATAL) << "Filter \"" << GetFilterId() << "\" cannot run as a global pre-filter, since it needs data from the product!";
	}

	virtual bool DoesEventPassLocal(event_type const& event,
			product_type const& product,
            setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if (product.selectedHltName.empty()) {
			// no HLT found
			return false;
		}

		// TODO: Report that we changed the HLT, if we did
		// LOG(DEBUG) << "Using trigger " << curName << ".";
		return event.m_eventMetadata->hltFired(product.selectedHltName, event.m_lumiMetadata);
	}
};

