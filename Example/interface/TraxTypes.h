/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalProducerBase.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "TraxEvent.h"
#include "TraxProduct.h"
#include "TraxProduct.h"
#include "TraxPipelineSettings.h"

// all data types which are used for this analysis
struct TraxTypes {
	typedef TraxEvent event_type;
	typedef TraxLocalProduct local_product_type;
	typedef TraxGlobalProduct global_product_type;
	typedef TraxPipelineSettings setting_type;
	typedef TraxGlobalSettings global_setting_type;
};

// Pass the template parameters for the Producers
typedef GlobalProducerBase<TraxTypes> TraxGlobalProducerBase;
typedef LocalProducerBase<TraxTypes> TraxLocalProducerBase;

// Pass the template parameters for the Consumer
typedef ConsumerBase<TraxTypes> TraxConsumerBase;

// Pass the template parameters for the Filter
typedef FilterBase<TraxTypes> TraxFilterBase;

//Pass the template parameters for the Pipeline
typedef Pipeline<TraxTypes> TraxPipeline;

//Setup our custom pipeline runner
typedef PipelineRunner<TraxPipeline, TraxGlobalProducerBase> TraxPipelineRunner;

