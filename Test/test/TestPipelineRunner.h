/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Pipeline.h"
#include "Artus/Core/interface/PipelineRunner.h"

#include "TestTypes.h"
#include "TestPipeline.h"

typedef PipelineRunner<TestPipeline, TestTypes > TestPipelineRunner;
typedef PipelineRunner<TestPipelineInstr, TestTypes > TestPipelineRunnerInstr;


typedef PipelineInitilizerBase<TestTypes> TestPipelineInitializer;
