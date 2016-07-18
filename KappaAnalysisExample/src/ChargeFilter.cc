
#include "Artus/KappaAnalysisExample/interface/ChargeFilter.h"


	
std::string ChargeFilter::GetFilterId() const
{
	return "ChargeFilter";
}

bool ChargeFilter::DoesEventPass(event_type const& event, product_type const& product,
                                 setting_type const& settings) const
{
	// make sure that there are at least two muons reconstructed
	// this should be ensured by a muon counting filter before running this producer
	assert(product.m_validMuons.size() >= 2);
	
	int chargeProduct = product.m_validMuons[0]->charge() * product.m_validMuons[1]->charge();
	return (settings.GetOSMuons() ? (chargeProduct < 0) : (chargeProduct > 0));
}

