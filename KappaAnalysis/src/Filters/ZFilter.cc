#include "Artus/KappaAnalysis/interface/Filters/ZFilter.h"


bool ZFilter::DoesEventPass(KappaEvent const& event,
                            KappaProduct const& product,
                            KappaSettings const& settings) const
{
	return product.m_zValid;
}
