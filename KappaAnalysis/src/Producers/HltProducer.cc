
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"

std::string HltProducer::GetProducerId() const {
	return "HltProducer";
}

void HltProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_lumiInfo);
	assert(event.m_eventInfo);
	
	if (product.m_settingsHltPaths.empty())
	{
		product.m_settingsHltPaths.insert(product.m_settingsHltPaths.begin(),
		                                  settings.GetHltPaths().begin(),
		                                  settings.GetHltPaths().end());
	}
	if (product.m_settingsHltPaths.empty()) {
		LOG(FATAL) << "No Hlt Trigger path list (tag \"HltPaths\") configured!";
	}

	// set LumiMetadat, needs to be done here for the case running over multiple files
	product.m_hltInfo.setLumiInfo(event.m_lumiInfo);

	// search trigger with lowest prescale
	std::string lowestPrescaleHltName;
	int lowestPrescale = std::numeric_limits<int>::max();

	std::vector<std::string> firedHltName;
	std::vector<int> prescaleFiredHlt;

	for (stringvector::const_iterator hltPath = product.m_settingsHltPaths.begin(); hltPath != product.m_settingsHltPaths.end(); ++hltPath)
	{
		std::string hltName = product.m_hltInfo.getHLTName(*hltPath);
		if (! hltName.empty())
		{
			// look for trigger with lowest prescale
			int prescale = product.m_hltInfo.getPrescale(hltName);
			if (prescale < lowestPrescale)
			{
				lowestPrescale = prescale;
				lowestPrescaleHltName = hltName;
			}
			
			// look for fired trigger
			if (event.m_eventInfo->hltFired(hltName, event.m_lumiInfo))
			{
				prescaleFiredHlt.push_back(prescale);
				firedHltName.push_back(hltName);
			}
		}
	}

 	for (unsigned int iHlt = 0; iHlt < firedHltName.size(); iHlt++)
	{
		std::string selectedHltName;
		double hltPrescaleWeight = 1.0;
	
		if (! product.m_hltInfo.isPrescaled(firedHltName.at(iHlt)))
		{
			if (prescaleFiredHlt.at(iHlt) == lowestPrescale)
			{
				selectedHltName = firedHltName.at(iHlt);
				hltPrescaleWeight = prescaleFiredHlt.at(iHlt);
			}
			else
			{
				selectedHltName = lowestPrescaleHltName;
				hltPrescaleWeight = lowestPrescale;
			}
		}
	
		if (settings.GetAllowPrescaledTrigger() && prescaleFiredHlt.at(iHlt) > lowestPrescale && (! firedHltName.empty()))
		{
			selectedHltName = firedHltName.at(iHlt);
			hltPrescaleWeight = prescaleFiredHlt.at(iHlt);
		}
	
		if (hltPrescaleWeight <= 0.0)
		{
			hltPrescaleWeight = 1.0;
		}
	
		product.m_selectedHltName.push_back(selectedHltName);
		product.m_weights["hltPrescaleWeight"] = hltPrescaleWeight;
		//TODO: how to define the HLT prescale eventweight when more than one HLT fires? The product of them? The min. or max. value?  
	
		if (! selectedHltName.empty())
		{
			product.m_selectedHltPosition.push_back((int) product.m_hltInfo.getHLTPosition(selectedHltName));
		}
		else
		{
			product.m_selectedHltPosition.push_back(DefaultValues::UndefinedInt);
		}
	}
}
