
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

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = globalSettings.GetHltPaths();
		Produce(event, product, hltPaths);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		std::vector<std::string> hltPaths = settings.GetHltPaths();
		Produce(event, product, hltPaths);
	}


private:

	// function that lets this producer work as both a global and a local producer
	void Produce(event_type const& event, product_type& product, std::vector<std::string> hltPaths) const
	{
		if (hltPaths.size() == 0)
			LOG(FATAL) << "No Hlt Trigger path list (tag \"HltPaths\") configured!";

		// set LumiMetadat, needs to be done here for the case running over multiple files
		product.m_hltInfo->setLumiMetadata(event.m_lumiMetadata);

		// search trigger with lowest prescale
		bool unprescaledPathFound = false;
		std::string bestHltName;
		int bestHltPrescale = std::numeric_limits<int>::max();
		for (stringvector::const_iterator hltPath = hltPaths.begin(); hltPath != hltPaths.end(); ++hltPath)
		{
			std::string hltName = product.m_hltInfo->getHLTName(*hltPath);
			LOG(DEBUG) << *hltPath << " becomes " << hltName << ".";

			if (! hltName.empty() && product.m_hltInfo->getPrescale(hltName) < bestHltPrescale)
			{
				bestHltName = hltName;
				bestHltPrescale = product.m_hltInfo->getPrescale(hltName);
				if (! product.m_hltInfo->isPrescaled(hltName))
				{
					unprescaledPathFound = true;
					break;
				}
			}
		}
		product.selectedHltName = bestHltName;
		
		// logging output
		if (bestHltName.empty()) {
			LOG(INFO) << "Looking for trigger paths";
			for (stringvector::const_iterator hltPath = hltPaths.begin(); hltPath != hltPaths.end(); ++hltPath)
			{
				LOG(INFO) << "\t" << (*hltPath);
			}
			LOG(INFO) << "Available triggers:";

			for (std::vector<std::string>::const_iterator hltName = event.m_lumiMetadata->hltNames.begin();
				 hltName != event.m_lumiMetadata->hltNames.end(); ++ hltName)
			{
				LOG(INFO) << "\t" << (*hltName) << ", prescale: " << product.m_hltInfo->getPrescale(*hltName);
			}
			LOG(FATAL) << "No trigger found for event " << event.m_eventMetadata->nRun << "!";
		}

		if (! unprescaledPathFound)
		{
			LOG(WARNING) << "No unprescaled trigger found for event " << event.m_eventMetadata->nRun
			             << "! Selected \"" << bestHltName << "\" (prescale " << bestHltPrescale << ").";
		}

		LOG(DEBUG) << "Selected " << bestHltName << " as best HLT, prescale: " << product.m_hltInfo->getPrescale(bestHltName) << ".";
	}

};

