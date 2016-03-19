
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
	
	// reset the values of crossSectionPerEventWeight and numberGeneratedEventsWeight to default, since the stitching weight is
	// computed taking into account those numbers
	product.m_weights["crossSectionPerEventWeight"] = 1.0;
	product.m_weights["numberGeneratedEventsWeight"] = 1.0;
	
	int nPartons = event.m_genEventInfo->lheNOutPartons;
	if (boost::regex_search(product.m_nickname, boost::regex("DY.?JetsToLLM(50|150)", boost::regex::icase | boost::regex::extended)))
	{
		if (product.m_genDiLeptonBoson.mass() < 150.0)
		{
			if ((nPartons == 0) || (nPartons >= 5))
			{
				product.m_optionalWeights["stitchWeightZLL"] = 2.43669e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 2.43669e-5;
			}
			else if (nPartons == 1)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.06292e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.06292e-5;
			}
			else if (nPartons == 2)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.10505e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.10505e-5;
			}
			else if (nPartons == 3)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.14799e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.14799e-5;
			}
			else
			{
				product.m_optionalWeights["stitchWeightZLL"] = 9.62135e-6;
				product.m_optionalWeights["stitchWeightZTT"] = 9.62135e-6;
			}
		}
		else
		{
			if ((nPartons == 0) || (nPartons >= 5))
			{
				product.m_optionalWeights["stitchWeightZLL"] = 2.43669e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.26276e-6;
			}
			else if (nPartons == 1)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.06292e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.18349e-6;
			}
			else if (nPartons == 2)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.10505e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.18854e-6;
			}
			else if (nPartons == 3)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.14799e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.19334e-6;
			}
			else
			{
				product.m_optionalWeights["stitchWeightZLL"] = 9.62135e-6;
				product.m_optionalWeights["stitchWeightZTT"] = 1.16985e-6;

			}
		}
	}
	else
	{
		LOG(FATAL) << "Stitching weights not implemented for nickname " << product.m_nickname << ": check config settings!";
	}
}
