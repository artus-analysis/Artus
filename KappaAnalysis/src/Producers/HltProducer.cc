
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"

std::string HltProducer::GetProducerId() const {
	return "HltProducer";
}

void HltProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "nSelectedHltPaths", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return static_cast<int>(product.m_selectedHltNames.size());
	});
	LambdaNtupleConsumer<KappaTypes>::AddVStringQuantity(metadata, "selectedHltPaths", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_selectedHltNames;
	});

	//std::vector<std::string> triggerDiscriminators;
	//triggerDiscriminators.push_back("HLT_Ele35_WPTight_Gsf_v7"); //used in et by arthur and hale

	for (std::string triggerDiscriminator : settings.GetHltPaths())
	{
		LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, triggerDiscriminator, [triggerDiscriminator](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
		{

			if (std::find(product.m_selectedHltPaths.begin(), product.m_selectedHltPaths.end(), triggerDiscriminator) != product.m_selectedHltPaths.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		});
	}
}

void HltProducer::Produce(event_type const& event, product_type& product,
                          setting_type const& settings, metadata_type const& metadata) const
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
	m_hltInfo.setLumiInfo(event.m_lumiInfo);

	// search (independently) for trigger with lowest prescale and for fired triggers
	std::string lowestPrescaleHltName;
	int lowestPrescale = std::numeric_limits<int>::max();

	std::string lowestSelectedPrescaleHltName;
	int lowestSelectedPrescale = std::numeric_limits<int>::max();

	product.m_selectedHltNames.clear();
	product.m_selectedHltPaths.clear();
	product.m_selectedHltPositions.clear();
	product.m_selectedHltPrescales.clear();
	for (std::vector<std::string>::const_iterator hltPath = product.m_settingsHltPaths.begin(); hltPath != product.m_settingsHltPaths.end(); ++hltPath)
	{
		std::string hltName = m_hltInfo.getHLTName(*hltPath);
		product.m_HltPathsNamesMap.insert(std::make_pair(hltName,*hltPath));
		if (! hltName.empty())
		{
			// look for trigger with lowest prescale
			// do not use hltName here as a parameter because *hltPath is already cached.
			int prescale = m_hltInfo.getPrescale(*hltPath);
			if ((prescale < lowestPrescale) && (prescale > 0))
			{
				lowestPrescale = prescale;
				lowestPrescaleHltName = hltName;
			}

			// look for (unprescaled if requested) fired trigger
			if ((event.m_eventInfo->hltFired(hltName, event.m_lumiInfo) || !settings.GetRequireFiredHlt()) && (settings.GetAllowPrescaledTrigger() || (prescale <= 1)))
			{
				product.m_selectedHltNames.push_back(hltName);
				product.m_selectedHltPaths.push_back(*hltPath);
				// do not use hltName here as a parameter because *hltPath is already cached.
				product.m_selectedHltPositions.push_back(static_cast<int>(m_hltInfo.getHLTPosition(*hltPath)));

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
