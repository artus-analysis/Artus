
#pragma once

#include "Artus/KappaAnalysis/interface/KappaEvent.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"
#include "Artus/KappaAnalysis/interface/KappaSettings.h"

#include "Artus/Core/interface/Pipeline.h"
#include "Artus/Core/interface/PipelineRunner.h"


struct KappaTypes { 
	typedef KappaEvent event_type;
	typedef KappaProduct product_type;
	typedef KappaSettings setting_type;
};

typedef Pipeline<KappaTypes> KappaPipeline;
typedef PipelineRunner<KappaPipeline, KappaTypes> KappaPipelineRunner;
typedef PipelineInitilizerBase<KappaTypes> KappaPipelineInitializer;
