
#pragma once

#include "TraxTypes.h"

#include "TraxPipelineSettings.h"

class TraxPipelineInitializer: public PipelineInitilizerBase<TraxTypes > {
public:

	void InitPipeline(TraxPipeline * pLine,
			setting_type const& settings, metadata_type& metadata) const override;
};
