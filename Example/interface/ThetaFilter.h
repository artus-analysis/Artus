
#pragma once

#include "TraxTypes.h"

class ThetaFilter: public TraxFilterBase {
public:

	virtual ~ThetaFilter();

	std::string GetFilterId() const override;

	bool DoesEventPass(TraxEvent const& event,
			TraxProduct const& product,
			TraxSettings const& globalSettings) const override;

};


