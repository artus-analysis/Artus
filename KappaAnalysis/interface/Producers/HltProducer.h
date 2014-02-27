
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/GlobalProducerBase.h"


/** 
 *
 *	
 */
template<class TTypes>
class HltProducer: public GlobalProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual std::string GetProducerId() {
		return "hlt_selector";
	}
	
	HltProducer() : GlobalProducerBase<TTypes>() {};

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		if (globalSettings.GetVerbose())
		{
			for (std::vector< std::string >::const_iterator it = event.m_lumiMetadata->hltNames.begin();
				 it != event.m_lumiMetadata->hltNames.end(); ++it)
			{
				std::cout << *it << std::endl;
			}
		}

		std::vector<std::string> hltPaths = globalSettings.GetHltPaths();
		bool unprescaledPathFound = false;
		std::string bestHltName, curName;

		if (hltPaths.size() == 0)
			LOG_FATAL("No Hlt Trigger path list configured");

		for (stringvector::const_iterator hltPath = hltPaths.begin(); hltPath != hltPaths.end(); ++hltPath)
		{
			curName = product.m_hltInfo->getHLTName(*hltPath);

			if (globalSettings.GetVerbose())
				std::cout << *hltPath << " becomes " << curName << std::endl;

			if (! curName.empty())
			{
				bestHltName = curName;
				if (! product.m_hltInfo->isPrescaled(curName))
				{
					unprescaledPathFound = true;
					break;
				}
			}
		}

		if (! unprescaledPathFound)
		{
			LOG("Available Triggers:");

			for (std::vector<std::string>::const_iterator hltName = event.m_lumiMetadata->hltNames.begin();
				 hltName != event.m_lumiMetadata->hltNames.end(); ++ hltName)
			{
				LOG((*hltName) << " prescale: " << product.m_hltInfo->getPrescale(*hltName));
			}

			LOG_FATAL("No unprescaled trigger found for " << bestHltName << ", prescale: " << product.m_hltInfo->getPrescale(bestHltName) << ", event: " << event.m_eventMetadata->nRun);
		}

		if (globalSettings.GetVerbose())
			LOG("selected " << bestHltName << " as best HLT, prescale: " << product.m_hltInfo->getPrescale(bestHltName));

		if (bestHltName.empty())
			LOG_FATAL("No HLT trigger path found at all!");

		product.selectedHltName = bestHltName;
		return true;
	}
};

