
#pragma once

#include <limits>

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/ProducerBase.h"


/** 
 *
 *	
 */
template<class TTypes>
class HltProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "hlt_selector";
	}
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitGlobal(globalSettings);
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitLocal(settings);
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = globalSettings.GetHltPaths();
		bool allowPrescaledTrigger = globalSettings.GetAllowPrescaledTrigger();
		Produce(event, product, hltPaths, allowPrescaledTrigger);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = settings.GetHltPaths();
		bool allowPrescaledTrigger = settings.GetAllowPrescaledTrigger();
		Produce(event, product, hltPaths, allowPrescaledTrigger);
	}


private:

	// function that lets this producer work as both a global and a local producer
	void Produce(event_type const& event, product_type& product,
	             std::vector<std::string> hltPaths, bool allowPrescaledTrigger) const
	{
		if (hltPaths.size() == 0) {
			LOG(FATAL) << "No Hlt Trigger path list (tag \"HltPaths\") configured!";
		}

		// set LumiMetadat, needs to be done here for the case running over multiple files
		product.m_hltInfo->setLumiMetadata(event.m_lumiMetadata);

		// search trigger with lowest prescale
		std::string lowestPrescaleHltName;
		int lowestPrescale = std::numeric_limits<int>::max();
		std::string firedTriggerName;
		int prescaleFiredHlt = std::numeric_limits<int>::max();
		bool unprescaledPathFound = false;
		for (stringvector::const_iterator hltPath = hltPaths.begin(); hltPath != hltPaths.end(); ++hltPath)
		{
			std::string hltName = product.m_hltInfo->getHLTName(*hltPath);
			if (! hltName.empty())
			{
				// look for trigger with lowest prescale
				int prescale = product.m_hltInfo->getPrescale(hltName);
				if (prescale < lowestPrescale)
				{
					lowestPrescaleHltName = prescale;
					lowestPrescaleHltName = hltName;
				}
				
				if (! product.m_hltInfo->isPrescaled(hltName))
				{
					unprescaledPathFound = true;
				}
				
				// look for fired trigger
				if (event.m_eventMetadata->hltFired(hltName, event.m_lumiMetadata))
				{
					if (prescale < prescaleFiredHlt) {
						firedTriggerName = hltName;
						prescaleFiredHlt = prescale;
					}
				}
				
				// stop searching when unprescaled and fired trigger is found
				if (unprescaledPathFound && prescaleFiredHlt == lowestPrescaleHltName)
				{
					break;
				}
			}
		}
		
		if (unprescaledPathFound || (allowPrescaledTrigger && (! lowestPrescaleHltName.empty())))
		{
			if (prescaleFiredHlt == lowestPrescaleHltName || allowPrescaledTrigger)
			{
				product.selectedHltName = firedTriggerName;
				product.m_weights["hltPrescaleWeight"] = prescaleFiredHlt;
			}
			else
			{
				product.selectedHltName = lowestPrescaleHltName;
				product.m_weights["hltPrescaleWeight"] = lowestPrescale;
			}
		}
		else
		{
			product.m_weights["hltPrescaleWeight"] = 1.0;
		}
		
	}

};

