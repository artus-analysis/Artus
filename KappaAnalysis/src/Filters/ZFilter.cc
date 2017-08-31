#include "Artus/KappaAnalysis/interface/Filters/ZFilter.h"

std::string ZFilter::GetFilterId() const
{
	return "ZFilter";
}

ZFilter::ZFilter() :
		FilterBase<KappaTypes>()
{
}

bool ZFilter::DoesEventPass(event_type const& event, product_type const& product,
                            setting_type const& settings, metadata_type const& metadata) const
{
	return product.m_zValid;
}
