
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"

std::string HltProducer::GetProducerId() const {
	return "HltProducer";
}

void HltProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nSelectedHltPaths", [](KappaEvent const& event, KappaProduct const& product)
	{
		return static_cast<int>(product.m_selectedHltNames.size());
	});
	LambdaNtupleConsumer<KappaTypes>::AddVStringQuantity("selectedHltPaths", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_selectedHltNames;
	});
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

	// search (independently) for trigger with lowest prescale and for fired triggers
	std::string lowestPrescaleHltName;
	int lowestPrescale = std::numeric_limits<int>::max();
	
	std::string lowestSelectedPrescaleHltName;
	int lowestSelectedPrescale = std::numeric_limits<int>::max();
	
	product.m_selectedHltNames.clear();
	product.m_selectedHltPositions.clear();
	product.m_selectedHltPrescales.clear();
	for (stringvector::const_iterator hltPath = product.m_settingsHltPaths.begin(); hltPath != product.m_settingsHltPaths.end(); ++hltPath)
	{
		std::string hltName = product.m_hltInfo.getHLTName(*hltPath);
		if (! hltName.empty())
		{
			// look for trigger with lowest prescale
			// do not use hltName here as a parameter because *hltPath is already cached.
			int prescale = product.m_hltInfo.getPrescale(*hltPath);
			if ((prescale < lowestPrescale) && (prescale > 0))
			{
				lowestPrescale = prescale;
				lowestPrescaleHltName = hltName;
			}
			
			// look for (unprescaled if requested) fired trigger
			if (event.m_eventInfo->hltFired(hltName, event.m_lumiInfo) && (settings.GetAllowPrescaledTrigger() || (prescale <= 1)))
			{
				product.m_selectedHltNames.push_back(hltName);
				
				// do not use hltName here as a parameter because *hltPath is already cached.
				product.m_selectedHltPositions.push_back(static_cast<int>(product.m_hltInfo.getHLTPosition(*hltPath)));
				
				product.m_selectedHltPrescales.push_back(prescale);
				if ((prescale < lowestSelectedPrescale) && (prescale > 0))
				{
					lowestSelectedPrescale = prescale;
					lowestSelectedPrescaleHltName = hltName;
				}
			}
		}
	}
	
	if ((! settings.GetAllowPrescaledTrigger()) && (lowestPrescale > 1))
	{
		LOG(WARNING) << "No unprescaled trigger found for event " << event.m_eventInfo->nEvent
		             << "! Lowest prescale: " << lowestPrescale << " (\"" << lowestPrescaleHltName << "\").";
	}
	
	if (! (lowestPrescale > 0))
	{
		lowestPrescale = 1;
	}
	if (! (lowestSelectedPrescale > 0) || (lowestSelectedPrescale == std::numeric_limits<int>::max()))
	{
		lowestSelectedPrescale = 1;
	}

	// TODO: how to define the HLT prescale eventweight when more than one HLT fires? The product of them? The min. or max. value? Maybe overwrite it later?
	product.m_weights["hltPrescaleWeight"] = lowestSelectedPrescale;
}
