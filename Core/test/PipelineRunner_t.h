/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "TestPipeline.h"
#include "TestGlobalProducer.h"
#include "TestEventProvider.h"
#include "Pipeline_t.h"

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_event_prunner_global_product )
{
	TestPipeline * tline1 = new TestPipeline;
	TestPipeline * tline2 = new TestPipeline;
	TestPipeline * tline3 = new TestPipeline;
	TestPipeline * tline4 = new TestPipeline;
	TestPipeline * tline5 = new TestPipeline;

	tline1->bCheckProducer = true;
	tline2->bCheckProducer = true;
	tline3->bCheckProducer = true;
	tline4->bCheckProducer = true;
	tline5->bCheckProducer = true;


	TestSettings tset;
	TestGlobalSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );
	tline2->InitPipeline( tset, TestPipelineInitilizer() );
	tline3->InitPipeline( tset, TestPipelineInitilizer() );
	tline4->InitPipeline( tset, TestPipelineInitilizer() );
	tline5->InitPipeline( tset, TestPipelineInitilizer() );

	PipelineRunner<TestPipeline, TestProducerBase> prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	prunner.AddGlobalProducer( new TestGlobalProducer() );

	std::vector<TestPipeline *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines<TestTypes>( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);

}

BOOST_AUTO_TEST_CASE( test_event_prunner )
{
	TestPipeline * tline1 = new TestPipeline;
	TestPipeline * tline2 = new TestPipeline;
	TestPipeline * tline3 = new TestPipeline;
	TestPipeline * tline4 = new TestPipeline;
	TestPipeline * tline5 = new TestPipeline;

	TestSettings tset;
	TestGlobalSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );
	tline2->InitPipeline( tset, TestPipelineInitilizer() );
	tline3->InitPipeline( tset, TestPipelineInitilizer() );
	tline4->InitPipeline( tset, TestPipelineInitilizer() );
	tline5->InitPipeline( tset, TestPipelineInitilizer() );

	PipelineRunner<TestPipeline, TestProducerBase> prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	std::vector<TestPipeline *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines<TestTypes>( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_multi_level )
{
	TestPipeline * tline1 = new TestPipeline;
	TestPipeline * tline2 = new TestPipeline;
	TestPipeline * tline3 = new TestPipeline;

	TestSettings tset_lvl1;
	TestSettings tset_lvl2;
	TestGlobalSettings global_tset_lvl2;
	tset_lvl1.SetLevel(1);
	tset_lvl2.SetLevel(2);
	tline1->InitPipeline( tset_lvl1, TestPipelineInitilizer() );
	tline2->InitPipeline( tset_lvl1, TestPipelineInitilizer() );
	tline3->InitPipeline( tset_lvl2, TestPipelineInitilizer() );

	PipelineRunner<TestPipeline, TestProducerBase> prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();
	prunner.AddPipelines( { {tline1, tline2, tline3}});

	TestEventProvider evtProvider;
	// give any pipeline setting here: only the global producer will
	// read from the global settings ...
	prunner.RunPipelines<TestTypes>( evtProvider, global_tset_lvl2 );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	// everything after leve1 is not run on the events,
	// could be changed at a later stage
	tline3->CheckCalls(0,1);
}
