
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"

std::string CrossSectionWeightProducer::GetProducerId() const
{
	return "CrossSectionWeightProducer";
}

void CrossSectionWeightProducer::Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
{
	assert(event.m_genLumiInfo);
	
	if (static_cast<double>(settings.GetCrossSection()) > 0.0)
		product.m_weights["crossSectionPerEventWeight"] = settings.GetCrossSection();
	else if (event.m_genLumiInfo->xSectionExt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiInfo->xSectionExt;
	else if (event.m_genLumiInfo->xSectionInt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiInfo->xSectionInt;
	else
		LOG(ERROR) << "No CrossSection information found.";
}
