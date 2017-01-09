#include "Artus/KappaAnalysis/interface/Filters/ZFilter.h"

std::string ZFilter::GetFilterId() const
{
	return "ZFilter";
}

ZFilter::ZFilter() :
		FilterBase<KappaTypes>()
{
}

bool ZFilter::DoesEventPass(KappaEvent const& event,
                            KappaProduct const& product,
                            KappaSettings const& settings) const
{
	return product.m_zValid;
}
