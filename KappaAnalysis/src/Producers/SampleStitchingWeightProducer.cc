
#include "Artus/KappaAnalysis/interface/Producers/SampleStitchingWeightProducer.h"

#include "Artus/Utility/interface/SafeMap.h"

std::string SampleStitchingWeightProducer::GetProducerId() const
{
	return "SampleStitchingWeightProducer";
}

void SampleStitchingWeightProducer::Init(KappaSettings const& settings)
{
	stitchingWeightsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetStitchingWeights()),
																	stitchingWeightsByName);
}

void SampleStitchingWeightProducer::Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
{
	assert(event.m_genEventInfo != nullptr);
	
	if(SafeMap::GetWithDefault(product.m_weights, std::string("crossSectionPerEventWeight"), -1.) <= 0.)
	{
		LOG(FATAL) << "Cross section not available or 0. Make sure that CrossSectionWeightProducer is run before SampleStitchingWeightProducer!";
	}
	else if(SafeMap::GetWithDefault(product.m_weights, std::string("numberGeneratedEventsWeight"), -1.) <= 0.)
	{
		LOG(FATAL) << "Number of generated events not available or 0. Make sure that NumberGeneratedEventsWeightProducer is run before SampleStitchingWeightProducer!";
	}
	else
	{
		int nPartons = event.m_genEventInfo->lheNOutPartons >= 5 ? 0 : event.m_genEventInfo->lheNOutPartons;
		product.m_weights["sampleStitchingWeight"] = SafeMap::Get(stitchingWeightsByIndex, size_t(nPartons)).at(0) / product.m_weights["numberGeneratedEventsWeight"] / product.m_weights["crossSectionPerEventWeight"];
	}
}
