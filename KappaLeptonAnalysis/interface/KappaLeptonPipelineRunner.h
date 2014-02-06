
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

template<class TTypes, typename TPipeline, typename TGlobalProducer>
class KappaLeptonPipelineRunner : public PipelineRunner<TPipeline, TGlobalProducer> {

public:
	typedef typename TTypes::global_setting_type global_setting_type;

	KappaLeptonPipelineRunner(global_setting_type const& globalSettings) :
		PipelineRunner<TPipeline, TGlobalProducer>(),
		m_globalSettings(globalSettings)
	{
		AddGlobalProducersById();
	}
	
	
	// Overwrite this function to define your own order of calling the producers
	virtual void AddGlobalProducersById() {
	
		BOOST_FOREACH(std::string producerId, m_globalSettings.GetGlobalProducers())
		{
			if(producerId == ValidMuonsProducer<TTypes>().GetProducerId()) {
				this->AddGlobalProducer(new ValidMuonsProducer<TTypes>());
			}
			else {
				LOG_FATAL("Global producer \"" << producerId << "\" not found.");
			}
		}
	}

protected:
	global_setting_type m_globalSettings;

};

