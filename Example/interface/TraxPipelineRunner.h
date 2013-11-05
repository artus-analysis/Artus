#pragma once

#pragma once

#include "Artus/Core/interface/EventPipelineRunner.h"

#include "TraxEventData.h"
#include "TraxMetaData.h"
#include "TraxPipeline.h"
#include "TraxPipelineSettings.h"
#include "TraxGlobalMetaDataProducerBase.h"

typedef EventPipelineRunner<TraxPipeline, TraxGlobalMetaDataProducerBase> TraxPipelineRunner;
