
#pragma once

#include "TraxTypes.h"

class PtFilter: public TraxFilterBase
{
public:

	virtual ~PtFilter();

	std::string GetFilterId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	bool DoesEventPass(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) const override;
};

