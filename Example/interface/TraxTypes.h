
#pragma once

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "TraxEvent.h"
#include "TraxProduct.h"
#include "TraxPipelineSettings.h"

// all data types which are used for this analysis
struct TraxTypes {
	typedef TraxEvent event_type;
	typedef TraxProduct product_type;
	typedef TraxSettings setting_type;
};

// Pass the template parameters for the Producers
typedef ProducerBase<TraxTypes> TraxProducerBase;

// Pass the template parameters for the Consumer
typedef ConsumerBase<TraxTypes> TraxConsumerBase;

// Pass the template parameters for the Filter
typedef FilterBase<TraxTypes> TraxFilterBase;

//Pass the template parameters for the Pipeline
typedef Pipeline<TraxTypes> TraxPipeline;

//Setup our custom pipeline runner
typedef PipelineRunner<TraxPipeline, TraxTypes> TraxPipelineRunner;

