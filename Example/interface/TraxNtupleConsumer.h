
#pragma once

#include "Artus/Consumer/interface/NtupleConsumerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"

#include "TraxTypes.h"

class TraxNtupleConsumer: public NtupleConsumerBase<TraxTypes> {

public:
	std::string GetConsumerId() const override;

private:
	float returnvalue(std::string string, event_type const& event, product_type const& product) override;

};
