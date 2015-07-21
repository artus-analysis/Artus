#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"


/** Filter events for that a previously selected HLT trigger has fired.
 *
 *  The selected HLT trigger has to be defined in the product by the HltProducer.
 *  Therefore this filter cannot meaningfully run as a global pre-filter
 *  which gets an empty product.
 */
	std::string HltFilter::GetFilterId() const {
		return "HltFilter";
	}

	bool HltFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const
	{
		assert(event.m_lumiInfo);
		assert(event.m_eventInfo);
	
		if (product.m_selectedHltName.empty())
		{
			// no HLT found
			return false;
		}
		else if (product.m_weights.at("hltPrescaleWeight") < 1.01)
		{
			return event.m_eventInfo->hltFired(product.m_selectedHltName.at(0), event.m_lumiInfo);;
		}
		else
		{
			if (! settings.GetAllowPrescaledTrigger())
			{
				LOG(FATAL) << "No unprescaled trigger found for event " << event.m_eventInfo->nEvent
					       << "! Lowest prescale: " << product.m_weights.at("hltPrescaleWeight") << " (\"" << product.m_selectedHltName.at(0) << "\").";
			}
			return (settings.GetAllowPrescaledTrigger() && event.m_eventInfo->hltFired(product.m_selectedHltName.at(0), event.m_lumiInfo));
		}
	}
