
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidElectronsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidMuonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidTausProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"

/**
   \brief Class to manages all registered Pipelines and to connect them to the event.
   
   The KappaPipelineRunner is derived from the PipelineRunner of Artus/Core. It utilizes the 
   KappaEventProvider to load events and passes them to all registered Pipelines. The EventProvider 
   is passed to the function PipelineRunner::RunPipelines as an argument. In addition five Global-
   Producers are registered, which can generate Pipeline-independet products of the event. These 
   GlobalProducers are run before any pipeline is started and the generated data is passed on to 
   the pipelines. 

   The order of running is: GlobalProducers -> LocalProducers -> Filters -> Consumers.
*/

template<class TTypes, typename TPipeline, typename TGlobalProducer>
class KappaPipelineRunner : public PipelineRunner<TPipeline, TGlobalProducer> {

public:
	typedef typename TTypes::global_setting_type global_setting_type;

	KappaPipelineRunner(global_setting_type const& globalSettings) :
		PipelineRunner<TPipeline, TGlobalProducer>(),
		m_globalSettings(globalSettings)
	{

	}
	
	
	/// Overwrite this function to define your own order of calling the producers
	virtual void AddGlobalProducersById() {
	
		BOOST_FOREACH(std::string producerId, m_globalSettings.GetGlobalProducers())
		{
		        if(producerId == GenTauDecayProducer<TTypes>().GetProducerId()) {
		  		this->AddGlobalProducer(new GenTauDecayProducer<TTypes>());
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

