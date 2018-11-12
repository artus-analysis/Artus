
#include "boost/algorithm/string/join.hpp"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"


std::string EventWeightProducer::GetProducerId() const {
	return "EventWeightProducer";
}

EventWeightProducer::~EventWeightProducer()
{
	if (! m_weightNames.empty())
	{
		LOG(DEBUG) << "Constructed event weight from indidual weights ("
		           << boost::algorithm::join(m_weightNames, ", ")
		           << ") in the pipeline \"" << pipelineName << "\".";
	}
}

void EventWeightProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);
	pipelineName = settings.GetName();
}

void EventWeightProducer::Produce(event_type const& event, product_type& product,
                                  setting_type const& settings, metadata_type const& metadata) const
{
	double eventWeight = settings.GetBaseWeight();
	bool firstRun = m_weightNames.empty();

	// loop over all previously calculated weights and multiply them
	for(std::map<std::string, double>::const_iterator weight = product.m_weights.begin();
		weight != product.m_weights.end(); ++weight)
	{
		eventWeight *= weight->second;
		LOG(DEBUG) << weight->first <<" : " << weight->second << "    total eventweight : " << eventWeight << std::endl;
		if (firstRun)
		{
			m_weightNames.push_back(weight->first);
		}
	}

	product.m_weights[settings.GetEventWeight()] = eventWeight;
}


