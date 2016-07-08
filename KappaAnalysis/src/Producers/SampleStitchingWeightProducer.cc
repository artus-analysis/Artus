
#include "Artus/KappaAnalysis/interface/Producers/SampleStitchingWeightProducer.h"

#include "Artus/Utility/interface/SafeMap.h"

std::string SampleStitchingWeightProducer::GetProducerId() const
{
	return "SampleStitchingWeightProducer";
}

void SampleStitchingWeightProducer::Init(KappaSettings const& settings)
{
	stitchingWeightsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap((settings.*GetStitchingWeights)()),
																	stitchingWeightsByName);
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
	
	int nPartons = event.m_genEventInfo->lheNOutPartons >= 5 ? 0 : event.m_genEventInfo->lheNOutPartons;
	product.m_optionalWeights["stitchWeight"] = SafeMap::Get(stitchingWeightsByIndex, size_t(nPartons)).at(0);
	/*if (boost::regex_search(product.m_nickname, boost::regex("DY.?JetsToLLM(50|150)_RunIIFall15", boost::regex::icase | boost::regex::extended)))
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
	else if (boost::regex_search(product.m_nickname, boost::regex("DY.?JetsToLLM(50|150)_RunIISpring16", boost::regex::icase | boost::regex::extended)))
	{
		if (product.m_genDiLeptonBoson.mass() < 150.0)
		{
			if ((nPartons == 0) || (nPartons >= 5))
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.208008366e-4;
				product.m_optionalWeights["stitchWeightZTT"] = 1.208008366e-4;
			}
			else if (nPartons == 1)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.62772301e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.62772301e-5;
			}
			else if (nPartons == 2)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.75930713e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.75930713e-5;
			}
			else if (nPartons == 3)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.83929231e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.83929231e-5;
			}
			else
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.41810906e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.41810906e-5;
			}
		}
		else
		{
			if ((nPartons == 0) || (nPartons >= 5))
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.208008366e-4;
				product.m_optionalWeights["stitchWeightZTT"] = 1.3134972e-6;
			}
			else if (nPartons == 1)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.62772301e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.2277715e-6;
			}
			else if (nPartons == 2)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.75930713e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.2347374e-6;
			}
			else if (nPartons == 3)
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.83929231e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.2385174e-6;
			}
			else
			{
				product.m_optionalWeights["stitchWeightZLL"] = 1.41810906e-5;
				product.m_optionalWeights["stitchWeightZTT"] = 1.2142337e-6;

			}
		}
	}
	else if (boost::regex_search(product.m_nickname, boost::regex("W.?JetsToLNu_RunIIFall15", boost::regex::icase | boost::regex::extended)))
	{
		if ((nPartons == 0) || (nPartons >= 5))
		{
			product.m_optionalWeights["stitchWeightWJ"] = 1.3046006677e-3;
		}
		else if (nPartons == 1)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 2.162338159e-4;
		}
		else if (nPartons == 2)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 1.159006627e-4;
		}
		else if (nPartons == 3)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 5.82002641e-5;
		}
		else
		{
			product.m_optionalWeights["stitchWeightWJ"] = 6.27558901e-05;
		}
	}
	else if (boost::regex_search(product.m_nickname, boost::regex("W.?JetsToLNu_RunIISpring16", boost::regex::icase | boost::regex::extended)))
	{
		if ((nPartons == 0) || (nPartons >= 5))
		{
			product.m_optionalWeights["stitchWeightWJ"] = 2.192495462e-3;
		}
		else if (nPartons == 1)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 2.475489115e-4;
		}
		else if (nPartons == 2)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 1.213458342e-4;
		}
		else if (nPartons == 3)
		{
			product.m_optionalWeights["stitchWeightWJ"] = 5.716934e-5;
		}
		else
		{
			product.m_optionalWeights["stitchWeightWJ"] = 6.27887343e-5;
		}
	}
	else
	{
		LOG(FATAL) << "Stitching weights not implemented for nickname " << product.m_nickname << ": check config settings!";
	}*/
}
