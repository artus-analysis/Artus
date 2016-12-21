
#pragma once

#include "Artus/Core/interface/CutFlow.h"
#include "Artus/Consumer/interface/CutFlowConsumerBase.h"

#include "TraxTypes.h"


class CutFlowConsumer: public CutFlowConsumerBase< TraxTypes > {

public:
	void Finish(setting_type const& setting) override;

};
