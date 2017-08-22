
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"

std::string CrossSectionWeightProducer::GetProducerId() const
{
	return "CrossSectionWeightProducer";
}

void CrossSectionWeightProducer::Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
{
	assert(event.m_genRunInfo);
	
	if (static_cast<double>(settings.GetCrossSection()) > 0.0)
		product.m_weights["crossSectionPerEventWeight"] = settings.GetCrossSection();
	else if (event.m_genRunInfo->xSectionExt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genRunInfo->xSectionExt;
	else if (event.m_genRunInfo->xSectionInt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genRunInfo->xSectionInt;
	else
		LOG(ERROR) << "No CrossSection information found.";
}
