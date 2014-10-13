
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GenDiLeptonDecayModeFilter.h"


std::string GenDiLeptonDecayModeFilter::GetFilterId() const {
	return "GenDiLeptonDecayModeFilter";
}

void GenDiLeptonDecayModeFilter::Init(setting_type const& settings)
{
	FilterBase<KappaTypes>::Init(settings);
	
	m_genDiLeptonDecayMode = KappaEnumTypes::ToDiLeptonDecayMode(boost::algorithm::to_lower_copy(
			boost::algorithm::trim_copy(settings.GetGenDiLeptonDecayMode())
	));
}

bool GenDiLeptonDecayModeFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                                               KappaSettings const& settings) const 
{
	if (m_genDiLeptonDecayMode != KappaEnumTypes::DiLeptonDecayMode::LL)
	{
		return ((product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE) ||
		        (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) ||
		        (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT));
	}
	else {
		return (product.m_genDiLeptonDecayMode == m_genDiLeptonDecayMode);
	}
}

