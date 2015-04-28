
#include "Artus/KappaAnalysis/interface/Consumers/PrintHltConsumer.h"

PrintHltConsumer::PrintHltConsumer() : ConsumerBase<KappaTypes>()
{
}

std::string PrintHltConsumer::GetConsumerId() const
{
	return "PrintHltConsumer";
}

void PrintHltConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                            setting_type const& settings)
{
	// set LumiInfo, needs to be done here for the case running over multiple files
	product.m_hltInfo.setLumiInfo(event.m_lumiInfo);
	
	LOG(INFO) << "Run: " << event.m_eventInfo->nRun << ", Lumi: " << event.m_eventInfo->nLumi << ", Event: " << event.m_eventInfo->nEvent;
	
	// loop over all HLT paths
	for (std::vector<std::string>::const_iterator hltName = event.m_lumiInfo->hltNames.begin();
	     hltName != event.m_lumiInfo->hltNames.end(); ++hltName)
	{
		// hltName can be a regular expression (e.g. does not need to contain the actual version number
		// (in this example, this call is not really necessary)
		std::string hltPath = product.m_hltInfo.getHLTName(*hltName);
		size_t hltIndex = product.m_hltInfo.getHLTPosition(hltPath);
		LOG(INFO) << "  HLT path: " << hltPath << " (index " << hltIndex << ")";
		
		if (event.m_eventInfo->hltFired(hltPath, event.m_lumiInfo))
		{
			LOG(INFO) << "    HLT fired, Prescale: " << product.m_hltInfo.getPrescale(hltPath);
			
			// loop over all filters of this fired HLT
			for (size_t filterIndex = event.m_triggerObjectMetadata->getMinFilterIndex(hltIndex);
			     filterIndex < event.m_triggerObjectMetadata->getMaxFilterIndex(hltIndex); ++filterIndex)
			{
				std::string filterName = event.m_triggerObjectMetadata->toFilter[filterIndex];
				LOG(INFO) << "      Filter: " << filterName << " (index " << filterIndex << ")";
				
				// loop over all trigger objects of this filter
				for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[filterIndex].begin();
				     triggerObjectIndex != event.m_triggerObjects->toIdxFilter[filterIndex].end(); ++triggerObjectIndex)
				{
					KLV triggerObject = event.m_triggerObjects->trgObjects[*triggerObjectIndex];
					LOG(INFO) << "        Trigger object: (pt = " << triggerObject.p4.Pt() << ", eta = " << triggerObject.p4.Eta() << ", phi = " << triggerObject.p4.Phi() << ", mass = " << triggerObject.p4.mass() << ")";
				}
			}
		}
		
	}
	LOG(INFO) << "==================================================";
}

void PrintHltConsumer::Finish(setting_type const& settings)
{
}

