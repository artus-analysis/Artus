/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalMetaDataProducerBase.h"
#include "Artus/Core/interface/EventPipelineRunner.h"

#include "TraxEventData.h"
#include "TraxMetaData.h"
#include "TraxMetaData.h"
#include "TraxPipelineSettings.h"

struct TraxTypes {
	typedef TraxEventData event_type;
	typedef TraxLocalMetaData local_meta_type;
	typedef TraxGlobalMetaData global_meta_type;
	typedef TraxPipelineSettings setting_type;
	typedef TraxGlobalSettings global_setting_type;
};

typedef GlobalMetaDataProducerBase<TraxTypes> TraxGlobalMetaDataProducerBase;
typedef LocalMetaDataProducerBase<TraxTypes> TraxLocalMetaDataProducerBase;

typedef EventConsumerBase<TraxTypes> TraxConsumerBase;
typedef FilterBase<TraxTypes> TraxFilterBase;

typedef EventPipeline<TraxTypes> TraxPipeline;

typedef EventPipelineRunner<TraxPipeline, TraxGlobalMetaDataProducerBase> TraxPipelineRunner;

