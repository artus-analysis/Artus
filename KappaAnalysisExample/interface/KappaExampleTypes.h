
#pragma once

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Core/interface/Pipeline.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleEvent.h"
#include "Artus/KappaAnalysisExample/interface/KappaExampleProduct.h"
#include "Artus/KappaAnalysisExample/interface/KappaExampleSettings.h"


/// all data types which are used for KappaExample analyses
class KappaExampleTypes {

public:
	/// content of the kappa ntuple
	typedef KappaExampleEvent event_type;
	
	/// objects that have been produced for plotting within Artus (i.e. logic implemented at analysis level)
	typedef KappaExampleProduct product_type;
	
	/// configuration settings which are KappaExample specific
	typedef KappaExampleSettings setting_type;
};

typedef Pipeline<KappaExampleTypes> KappaExamplePipeline;
typedef PipelineRunner<KappaExamplePipeline, KappaExampleTypes> KappaExamplePipelineRunner;
typedef PipelineInitilizerBase<KappaExampleTypes> KappaExamplePipelineInitializer;

