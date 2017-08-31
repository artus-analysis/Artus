
#pragma once

#include "TraxTypes.h"

class ThetaFilter: public TraxFilterBase {
public:

	virtual ~ThetaFilter();

	std::string GetFilterId() const override;

	bool DoesEventPass(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) const override;

};


