
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
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TTypes>::InitGlobal(globalSettings);
		allowPrescaledTrigger = globalSettings.GetAllowPrescaledTrigger();
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TTypes>::InitLocal(settings);
		allowPrescaledTrigger = settings.GetAllowPrescaledTrigger();
	}

	virtual bool DoesEventPassLocal(event_type const& event,
			product_type const& product, setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		return DoesEventPass(event, product);
	}

	virtual bool DoesEventPassGlobal(event_type const& event, product_type const& product,
			global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		return DoesEventPass(event, product);
	}


protected:

	virtual bool DoesEventPass(event_type const& event, product_type const& product) const
	{
		if (product.selectedHltName.empty())
		{
			// no HLT found
			return false;
		}
		else if (product.m_weights.at("hltPrescaleWeight") < 1.01)
		{
			return event.m_eventMetadata->hltFired(product.selectedHltName, event.m_lumiMetadata);;
		}
		else
		{
			if (! allowPrescaledTrigger)
			{
				LOG(FATAL) << "No unprescaled trigger found for event " << event.m_eventMetadata->nEvent
					       << "! Lowest prescale: " << product.m_weights.at("hltPrescaleWeight") << " (\"" << product.selectedHltName << "\").";
			}
			return (allowPrescaledTrigger && event.m_eventMetadata->hltFired(product.selectedHltName, event.m_lumiMetadata));
		}
	}


private:
	bool allowPrescaledTrigger;

};

