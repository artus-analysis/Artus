
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
	if (boost::regex_search(product.m_nickname, boost::regex("DYJetsToLL", boost::regex::icase | boost::regex::extended)))
	{
		if (product.m_genDiLeptonBoson.mass() < 150.0)
		{
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
		else
		{
			if ((nPartons == 0) || (nPartons >= 5))
			{
				product.m_weights["stitchWeight"] = 0.00000132913;
			}
			else if (nPartons == 1)
			{
				product.m_weights["stitchWeight"] = 0.00000124160;
			}
			else if (nPartons == 2)
			{
				product.m_weights["stitchWeight"] = 0.00000124716;
			}
			else if (nPartons == 3)
			{
				product.m_weights["stitchWeight"] = 0.00000125244;
			}
			else
			{
				product.m_weights["stitchWeight"] = 0.00000122659;
			}
		}
	}
	else
	{
		LOG(FATAL) << "Stitching weights not implemented for nickname " << product.m_nickname << ": check config settings!";
	}
}
