
#pragma once

#include "TraxTypes.h"

class PtCorrectionProducerLocal: public TraxProducerBase {
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event,
			product_type & product,
			setting_type const& settings, metadata_type const& metadata) const override;

};
