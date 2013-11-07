/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#pragma once

#include "Artus/Core/interface/EventPipelineRunner.h"

#include "TraxEventData.h"
#include "TraxMetaData.h"
#include "TraxPipeline.h"
#include "TraxPipelineSettings.h"
#include "TraxGlobalMetaDataProducerBase.h"

typedef EventPipelineRunner<TraxPipeline, TraxGlobalMetaDataProducerBase> TraxPipelineRunner;
