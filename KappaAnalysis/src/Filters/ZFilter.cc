#include "Artus/KappaAnalysis/interface/Filters/ZFilter.h"


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
