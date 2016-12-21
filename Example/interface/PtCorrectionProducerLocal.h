
#pragma once

#include "TraxTypes.h"

class PtCorrectionProducerLocal: public TraxProducerBase {
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;

	void Produce(TraxEvent const& event,
			TraxProduct & product,
			TraxSettings const& localSettings) const override;

};
