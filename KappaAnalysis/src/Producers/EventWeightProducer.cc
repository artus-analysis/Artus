
#include "boost/algorithm/string/join.hpp"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"


std::string EventWeightProducer::GetProducerId() const {
	return "EventWeightProducer";
}

EventWeightProducer::~EventWeightProducer()
{
	LOG(DEBUG) << "Constructed event weight from indidual weights ("
	           << boost::algorithm::join(m_weightNames, ", ")
	           << ") in the pipeline \"" << pipelineName << "\".";
}

void EventWeightProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
	pipelineName = settings.GetName();
	m_baseWeight = settings.GetBaseWeight();
}

void EventWeightProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                  KappaSettings const& settings) const
{
	double eventWeight = m_baseWeight;
	bool firstRun = m_weightNames.empty();

	// loop over all previously calculated weights and multiply them
	for(std::map<std::string, double>::const_iterator weight = product.m_weights.begin();
		weight != product.m_weights.end(); ++weight)
	{
		eventWeight *= weight->second;
		
		if (firstRun)
		{
			m_weightNames.push_back(weight->first);
		}
	}

	product.m_weights[settings.GetEventWeight()] = eventWeight;
}


