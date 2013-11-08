/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalProducerBase.h"
#include "Artus/Core/interface/EventPipelineRunner.h"

#include "TraxEventData.h"
#include "TraxMetaData.h"
#include "TraxMetaData.h"
#include "TraxPipelineSettings.h"

// all data types which are used for this analysis
struct TraxTypes {
	typedef TraxEventData event_type;
	typedef TraxLocalMetaData local_meta_type;
	typedef TraxGlobalMetaData global_meta_type;
	typedef TraxPipelineSettings setting_type;
	typedef TraxGlobalSettings global_setting_type;
};

// Pass the template parameters for the Producers
typedef GlobalProducerBase<TraxTypes> TraxGlobalProducerBase;
typedef LocalMetaDataProducerBase<TraxTypes> TraxLocalMetaDataProducerBase;

// Pass the template parameters for the Consumer
typedef EventConsumerBase<TraxTypes> TraxConsumerBase;

// Pass the template parameters for the Filter
typedef FilterBase<TraxTypes> TraxFilterBase;

//Pass the template parameters for the Pipeline
typedef EventPipeline<TraxTypes> TraxPipeline;

//Setup our custom pipeline runner
typedef EventPipelineRunner<TraxPipeline, TraxGlobalProducerBase> TraxPipelineRunner;

