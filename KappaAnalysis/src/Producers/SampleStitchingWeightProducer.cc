
#include "Artus/KappaAnalysis/interface/Producers/SampleStitchingWeightProducer.h"

#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"


std::string SampleStitchingWeightProducer::GetProducerId() const
{
	return "SampleStitchingWeightProducer";
}

void SampleStitchingWeightProducer::Init(KappaSettings const& settings)
{
	stitchingWeightsByIndex = Utility::ParseMapTypes<size_t, double>(
			Utility::ParseVectorToMap(settings.GetStitchingWeights()),
			stitchingWeightsByName
	);
	stitchingWeightsHighMassByIndex = Utility::ParseMapTypes<size_t, double>(
			Utility::ParseVectorToMap(settings.GetStitchingWeightsHighMass()),
			stitchingWeightsHighMassByName
	);
}

void SampleStitchingWeightProducer::Produce(
		KappaEvent const& event,
		KappaProduct & product,
		KappaSettings const& settings
) const
{
	assert(event.m_genEventInfo != nullptr);
	
	if (! Utility::Contains(product.m_weights, std::string("crossSectionPerEventWeight")))
	{
		LOG(FATAL) << "Cross section not available or 0. Make sure that CrossSectionWeightProducer is run before SampleStitchingWeightProducer!";
	}
	if (! Utility::Contains(product.m_weights, std::string("numberGeneratedEventsWeight")))
	{
		LOG(FATAL) << "Number of generated events not available or 0. Make sure that NumberGeneratedEventsWeightProducer is run before SampleStitchingWeightProducer!";
	}
	
	size_t nPartons = event.m_genEventInfo->lheNOutPartons >= 5 ? 0 : event.m_genEventInfo->lheNOutPartons;
	
	// take overlap of phase space into account for DY samples with M50 & M150
	if ((product.m_genDiLeptonBoson.mass() >= 150.0) && (stitchingWeightsHighMassByIndex.size() > 0))
	{
		product.m_weights["sampleStitchingWeight"] = SafeMap::Get(stitchingWeightsHighMassByIndex, nPartons).at(0);
	}
	else
	{
		product.m_weights["sampleStitchingWeight"] = SafeMap::Get(stitchingWeightsByIndex, nPartons).at(0);
	}
	
	product.m_weights["sampleStitchingWeight"] /= (product.m_weights["numberGeneratedEventsWeight"] * product.m_weights["crossSectionPerEventWeight"]);
}
