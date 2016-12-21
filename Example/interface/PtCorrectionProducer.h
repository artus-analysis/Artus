
#pragma once

#include "TraxTypes.h"

class PtCorrectionProducer: public TraxProducerBase
{
public:

	std::string GetProducerId() const override;

	void Init(TraxSettings const& globalSettings) override;

	void Produce(TraxEvent const& event, TraxProduct & product,
			TraxSettings const& globalSettings) const override;

};
