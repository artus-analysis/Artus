#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


class BeamScrapingFilter: public FilterBase<KappaTypes>
{
public:
	virtual std::string GetFilterId() const override;
	virtual void Init(setting_type const& settings) override;
	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override;
private:
	double m_purityRatio;
};
