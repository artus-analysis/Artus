#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

#include "TraxPipelineSettings.h"
#include "TraxEventData.h"
#include "TraxMetaData.h"

// filter
#include "PtFilter.h"

// consumer
#include "MeanPtConsumer.h"

class TraxPipelineInitilizer: public PipelineInitilizerBase<TraxTypes> {
public:

	virtual void InitPipeline(TraxPipeline * pLine,
			TraxPipelineSettings const& pset) const ARTUS_CPP11_OVERRIDE
			{

		pLine->AddFilter(new PtFilter());

		pLine->AddConsumer(new MeanPtConsumer());
	}
};
