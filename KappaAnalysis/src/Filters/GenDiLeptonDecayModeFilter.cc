
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GenDiLeptonDecayModeFilter.h"


std::string GenDiLeptonDecayModeFilter::GetFilterId() const {
	return "GenDiLeptonDecayModeFilter";
}

void GenDiLeptonDecayModeFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	m_genDiLeptonDecayMode = KappaEnumTypes::ToDiLeptonDecayMode(boost::algorithm::to_lower_copy(
			boost::algorithm::trim_copy(settings.GetGenDiLeptonDecayMode())
	));
}

bool GenDiLeptonDecayModeFilter::DoesEventPass(event_type const& event, product_type const& product,
                                               setting_type const& settings, metadata_type const& metadata) const
{
	bool valid = true;
	if (m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::LL)
	{
		valid = ((product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE) ||
		         (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) ||
		         (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT));
	}
	// Sometimes status 3 bosons (e.g., Z) are removed from the list of gen particles in
	// MINIAOD events. This option ensures that the event is not filtered in case no boson
	// is found.
	else if (settings.GetGenDiLeptonDecayModeFilterLoose())
	{
		valid = ((product.m_genDiLeptonDecayMode == m_genDiLeptonDecayMode) ||
		         (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::NONE));
	}
	else {
		valid = (product.m_genDiLeptonDecayMode == m_genDiLeptonDecayMode);
	}
	return (valid != settings.GetInvertGenDiLeptonDecayMode());
}
