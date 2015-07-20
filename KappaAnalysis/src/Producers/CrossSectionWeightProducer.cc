
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"

std::string CrossSectionWeightProducer::GetProducerId() const
{
	return "CrossSectionWeightProducer";
}

void CrossSectionWeightProducer::Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
{
	assert(event.m_genLumiMetadata);
	
	if (static_cast<double>(settings.GetCrossSection()) > 0.0)
		product.m_weights["crossSectionPerEventWeight"] = settings.GetCrossSection();
	else if (event.m_genLumiMetadata->xSectionExt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiMetadata->xSectionExt;
	else if (event.m_genLumiMetadata->xSectionInt > 0.)
		product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiMetadata->xSectionInt;
	else
		LOG(ERROR) << "No CrossSection information found.";
}
