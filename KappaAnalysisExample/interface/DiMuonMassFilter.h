
#pragma once

#include "Artus/Core/interface/FilterBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class DiMuonMassFilter: public FilterBase<KappaExampleTypes> {
public:

	virtual std::string GetFilterId() const override;
    
	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings, metadata_type const& metadata) const override;

};


