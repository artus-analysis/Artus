
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "Artus/KappaAnalysis/interface/Producers/JsonFilter.h"
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidElectronsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidMuonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidTausProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"


template<class TTypes, typename TPipeline, typename TGlobalProducer>
class KappaPipelineRunner : public PipelineRunner<TPipeline, TGlobalProducer> {

public:
	typedef typename TTypes::global_setting_type global_setting_type;

	KappaPipelineRunner(global_setting_type const& globalSettings) :
		PipelineRunner<TPipeline, TGlobalProducer>(),
		m_globalSettings(globalSettings)
	{

	}
	
	
	// Overwrite this function to define your own order of calling the producers
	virtual void AddGlobalProducersById() {
	
		BOOST_FOREACH(std::string producerId, m_globalSettings.GetGlobalProducers())
		{
			if(producerId == JsonFilter<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new JsonFilter<TTypes>(m_globalSettings));
			}
			else if(producerId == HltProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new HltProducer<TTypes>());
			}
			else if(producerId == ValidElectronsProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new ValidElectronsProducer<TTypes>());
			}
			else if(producerId == ValidMuonsProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new ValidMuonsProducer<TTypes>());
			}
			else if(producerId == ValidTausProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new ValidTausProducer<TTypes>());
			}
			else if(producerId == ValidJetsProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new ValidJetsProducer<TTypes>());
			}
			else {
				LOG("Global producer \"" << producerId << "\" not found.");
			}
		}
	}

protected:
	global_setting_type m_globalSettings;

};

