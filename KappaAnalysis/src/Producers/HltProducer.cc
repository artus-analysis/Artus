
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"

std::string HltProducer::GetProducerId() const {
	return "HltProducer";
}

void HltProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	if (settings.GetHltPaths().size() == 0) {
		LOG(FATAL) << "No Hlt Trigger path list (tag \"HltPaths\") configured!";
	}

	// set LumiMetadat, needs to be done here for the case running over multiple files
	product.m_hltInfo.setLumiMetadata(event.m_lumiMetadata);

	// search trigger with lowest prescale
	std::string lowestPrescaleHltName;
	int lowestPrescale = std::numeric_limits<int>::max();
	std::string firedHltName;
	int prescaleFiredHlt = std::numeric_limits<int>::max();
	bool unprescaledPathFound = false;
	for (stringvector::const_iterator hltPath = settings.GetHltPaths().begin(); hltPath != settings.GetHltPaths().end(); ++hltPath)
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
			
			if (! product.m_hltInfo.isPrescaled(hltName))
			{
				unprescaledPathFound = true;
			}
			
			// look for fired trigger
			if (event.m_eventMetadata->hltFired(hltName, event.m_lumiMetadata))
			{
				if (prescale < prescaleFiredHlt) {
					prescaleFiredHlt = prescale;
					firedHltName = hltName;
				}
			}
			
			// stop searching when unprescaled and fired trigger is found
			if (unprescaledPathFound && prescaleFiredHlt == lowestPrescale)
			{
				break;
			}
		}
	}
	
	std::string selectedHltName;
	double hltPrescaleWeight = 1.0;
	
	if (unprescaledPathFound)
	{
		if (prescaleFiredHlt == lowestPrescale)
		{
			selectedHltName = firedHltName;
			hltPrescaleWeight = prescaleFiredHlt;
		}
		else
		{
			selectedHltName = lowestPrescaleHltName;
			hltPrescaleWeight = lowestPrescale;
		}
	}
	
	if (settings.GetAllowPrescaledTrigger() && prescaleFiredHlt > lowestPrescale && (! firedHltName.empty()))
	{
		selectedHltName = firedHltName;
		hltPrescaleWeight = prescaleFiredHlt;
	}
	
	if (hltPrescaleWeight <= 0.0)
	{
		hltPrescaleWeight = 1.0;
	}
	
	product.m_selectedHltName = selectedHltName;
	product.m_weights["hltPrescaleWeight"] = hltPrescaleWeight;
	
	if (! selectedHltName.empty())
	{
		product.m_selectedHltPosition = (int) product.m_hltInfo.getHLTPosition(selectedHltName);
	}
	else
	{
		product.m_selectedHltPosition = DefaultValues::UndefinedInt;
	}
}
