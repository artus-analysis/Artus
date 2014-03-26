
#pragma once

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
	
	HltProducer() : ProducerBase<TTypes>() {};

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = globalSettings.GetHltPaths();
		Produce(event, product, hltPaths, globalSettings.GetVerbose());
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = settings.GetHltPaths();
		Produce(event, product, hltPaths, settings.GetVerbose());
	}


private:

	// function that lets this producer work as both a global and a local producer
	void Produce(event_type const& event, product_type& product,
	             std::vector<std::string> hltPaths, bool verbose=false) const
	{
		if (verbose)
		{
			for (std::vector< std::string >::const_iterator it = event.m_lumiMetadata->hltNames.begin();
				 it != event.m_lumiMetadata->hltNames.end(); ++it)
			{
				std::cout << *it << std::endl;
			}
		}

		bool unprescaledPathFound = false;
		std::string bestHltName, curName;

		if (hltPaths.size() == 0)
			ARTUS_LOG_FATAL("No Hlt Trigger path list configured");

		for (stringvector::const_iterator hltPath = hltPaths.begin(); hltPath != hltPaths.end(); ++hltPath)
		{
			curName = product.m_hltInfo->getHLTName(*hltPath);

			if (verbose)
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
			ARTUS_LOG("Available Triggers:");

			for (std::vector<std::string>::const_iterator hltName = event.m_lumiMetadata->hltNames.begin();
				 hltName != event.m_lumiMetadata->hltNames.end(); ++ hltName)
			{
				ARTUS_LOG((*hltName) << " prescale: " << product.m_hltInfo->getPrescale(*hltName));
			}

			ARTUS_LOG_FATAL("No unprescaled trigger found for " << bestHltName << ", prescale: " << product.m_hltInfo->getPrescale(bestHltName) << ", event: " << event.m_eventMetadata->nRun);
		}

		if (verbose)
			ARTUS_LOG("selected " << bestHltName << " as best HLT, prescale: " << product.m_hltInfo->getPrescale(bestHltName));

		if (bestHltName.empty())
			ARTUS_LOG_FATAL("No HLT trigger path found at all!");

		product.selectedHltName = bestHltName;
	}

};

