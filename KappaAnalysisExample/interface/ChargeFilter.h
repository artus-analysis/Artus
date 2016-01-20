
#pragma once

#include "Artus/Core/interface/FilterBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class ChargeFilter: public FilterBase<KappaExampleTypes> {
public:

	typedef typename KappaExampleTypes::event_type event_type;
	typedef typename KappaExampleTypes::product_type product_type;
	typedef typename KappaExampleTypes::setting_type setting_type;
	
	virtual std::string GetFilterId() const override;
    
	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const override;

};


