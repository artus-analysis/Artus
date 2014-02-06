/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalProducerBase.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "KappaLeptonEvent.h"
#include "KappaLeptonProduct.h"
#include "KappaLeptonPipelineSettings.h"

// all data types which are used for this analysis
struct KappaLeptonTypes {
	typedef KappaLeptonEvent event_type;
	typedef KappaLeptonProduct product_type;
	typedef KappaLeptonPipelineSettings setting_type;
	typedef KappaLeptonGlobalSettings global_setting_type;
};

// Pass the template parameters for the Producers
typedef GlobalProducerBase<KappaLeptonTypes> KappaLeptonGlobalProducerBase;
typedef LocalProducerBase<KappaLeptonTypes> KappaLeptonLocalProducerBase;

// Pass the template parameters for the Consumer
typedef ConsumerBase<KappaLeptonTypes> KappaLeptonConsumerBase;

// Pass the template parameters for the Filter
typedef FilterBase<KappaLeptonTypes> KappaLeptonFilterBase;

//Pass the template parameters for the Pipeline
typedef Pipeline<KappaLeptonTypes> KappaLeptonPipeline;

//Setup our custom pipeline runner
//typedef PipelineRunner<KappaLeptonPipeline, KappaLeptonGlobalProducerBase> KappaLeptonPipelineRunner;

