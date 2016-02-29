
#include "Artus/KappaAnalysis/interface/Producers/SampleStitchingWeightProducer.h"

std::string SampleStitchingWeightProducer::GetProducerId() const
{
	return "SampleStitchingWeightProducer";
}

void SampleStitchingWeightProducer::Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
{
	assert(event.m_genEventInfo != nullptr);
	
	// reset the values of crossSectionPerEventWeight, generatorWeight and
	// numberGeneratedEventsWeight to default, since the stitching weight is
	// computed considering also those numbers
	product.m_weights["crossSectionPerEventWeight"] = 1.0;
	product.m_weights["generatorWeight"] = 1.0;
	product.m_weights["numberGeneratedEventsWeight"] = 1.0;
	
	int nPartons = event.m_genEventInfo->lheNOutPartons;
	
	if ((nPartons == 0) || (nPartons >= 5))
	{
		product.m_weights["stitchWeight"] = 0.0006691449;
	}
	else if (nPartons == 1)
	{
		product.m_weights["stitchWeight"] = 0.00001833676;
	}
	else if (nPartons == 2)
	{
		product.m_weights["stitchWeight"] = 0.00001962736;
	}
	else if (nPartons == 3)
	{
		product.m_weights["stitchWeight"] = 0.00002102429;
	}
	else
	{
		product.m_weights["stitchWeight"] = 0.00001553018;
	}
}
