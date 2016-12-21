
#pragma once

#include "TraxTypes.h"

#include "TraxPipelineSettings.h"
#include "TraxEvent.h"
#include "TraxProduct.h"

class TraxPipelineInitializer: public PipelineInitilizerBase<TraxTypes > {
public:

	void InitPipeline(TraxPipeline * pLine,
			TraxSettings const& pset) const override;
};
