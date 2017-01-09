
#pragma once

#include "Artus/Core/interface/Pipeline.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "TestTypes.h"
#include "TestPipeline.h"

typedef PipelineRunner<TestPipeline, TestTypes > TestPipelineRunner;
typedef PipelineRunner<TestPipelineInstr, TestTypes > TestPipelineRunnerInstr;


typedef PipelineInitilizerBase<TestTypes> TestPipelineInitializer;
