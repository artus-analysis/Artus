#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"


/** Filter events for that a previously selected HLT trigger has fired.
 *
 *  The selected HLT trigger has to be defined in the product by the HltProducer.
 *  Therefore this filter cannot meaningfully run as a global pre-filter
 *  which gets an empty product.
 */

bool HltFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                           KappaSettings const& settings) const
{
	if (settings.GetNoHltFiltering()) return true;
	return (! product.m_selectedHltNames.empty());
}
