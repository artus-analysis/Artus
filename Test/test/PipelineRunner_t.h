
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "TestPipeline.h"
#include "TestGlobalProducer.h"
#include "TestEventProvider.h"
#include "TestPipelineRunner.h"
#include "TestFilter.h"
#include "Pipeline_t.h"
#include "TestFactory.h"

#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( test_event_prunner_global_product )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;
	TestPipelineInstr * tline2 = new TestPipelineInstr;
	TestPipelineInstr * tline3 = new TestPipelineInstr;
	TestPipelineInstr * tline4 = new TestPipelineInstr;
	TestPipelineInstr * tline5 = new TestPipelineInstr;

	tline1->bCheckProducer = true;
	tline2->bCheckProducer = true;
	tline3->bCheckProducer = true;
	tline4->bCheckProducer = true;
	tline5->bCheckProducer = true;

	TestMetadata metadata;
	TestSettings global_tset;
	tline1->InitPipeline( TestSettings("1"), metadata, TestPipelineInitializer() );
	tline2->InitPipeline( TestSettings("2"), metadata, TestPipelineInitializer() );
	tline3->InitPipeline( TestSettings("3"), metadata, TestPipelineInitializer() );
	tline4->InitPipeline( TestSettings("4"), metadata, TestPipelineInitializer() );
	tline5->InitPipeline( TestSettings("5"), metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	prunner.AddProducer( new TestGlobalProducer() );

	std::vector<TestPipelineInstr *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_global_producer_filter )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;

	tline1->bCheckProducer = true;

	TestMetadata metadata;
	TestSettings tset("1");
	TestSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner(false);
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );

	prunner.AddProducer( new TestGlobalProducer() );
	// this filter will be false for all events
	prunner.AddFilter( new TestGlobalFilter2() );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	// pipelines will be executed, even though the global filter
	// did not all succeed
	tline1->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_global_producer_filter_pass )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;

	tline1->bCheckProducer = true;

	TestMetadata metadata;
	TestSettings tset("1");
	TestSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner(false);
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );

	prunner.AddProducer( new TestGlobalProducer() );
	prunner.AddFilter( new TestGlobalFilter() );
	// this filter will drop all events
	prunner.AddProducer( new TestGlobalProducer2() );
	prunner.AddFilter( new TestGlobalFilter2() );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	// pipeline should never have been called
	tline1->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;
	TestPipelineInstr * tline2 = new TestPipelineInstr;
	TestPipelineInstr * tline3 = new TestPipelineInstr;
	TestPipelineInstr * tline4 = new TestPipelineInstr;
	TestPipelineInstr * tline5 = new TestPipelineInstr;

	TestMetadata metadata;
	TestSettings global_tset;
	tline1->InitPipeline( TestSettings("1"), metadata, TestPipelineInitializer() );
	tline2->InitPipeline( TestSettings("2"), metadata, TestPipelineInitializer() );
	tline3->InitPipeline( TestSettings("3"), metadata, TestPipelineInitializer() );
	tline4->InitPipeline( TestSettings("4"), metadata, TestPipelineInitializer() );
	tline5->InitPipeline( TestSettings("5"), metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner(false);
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );

	std::vector<TestPipelineInstr *> vPipes;
	vPipes.push_back( tline3 );
	vPipes.push_back( tline4 );
	vPipes.push_back( tline5 );
	prunner.AddPipelines( vPipes );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_result )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;
	tline1->bFullyRun = true;

	TestPipelineInstr * tline2 = new TestPipelineInstr;
	tline2->bFullyRun = false;

	TestPipelineInstr * tline3 = new TestPipelineInstr;
	tline3->lmdRunEventCheck = [] ( TestProduct const& globalProduct )
	{
		BOOST_CHECK( globalProduct.PreviousPipelinesResult.GetFilterDecision( "line1" )
				== FilterResult::Decision::Passed );
		BOOST_CHECK( globalProduct.PreviousPipelinesResult.GetFilterDecision( "line2" )
						== FilterResult::Decision::NotPassed );
		BOOST_CHECK( globalProduct.PreviousPipelinesResult.GetFilterDecision( "line3" )
						== FilterResult::Decision::Undefined );
	};

	TestMetadata metadata;
	TestSettings global_tset;
	tline1->InitPipeline( TestSettings("line1"), metadata, TestPipelineInitializer() );
	tline2->InitPipeline( TestSettings("line2"), metadata, TestPipelineInitializer() );
	tline3->InitPipeline( TestSettings("line3"), metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner(false);
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );
	prunner.AddPipeline( tline2 );
	prunner.AddPipeline( tline3 );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_multi_level )
{
	TestPipelineInstr * tline1 = new TestPipelineInstr;
	TestPipelineInstr * tline2 = new TestPipelineInstr;
	TestPipelineInstr * tline3 = new TestPipelineInstr;

	TestMetadata metadata;
	TestSettings tset_lvl1_a( "lvl1a");
	TestSettings tset_lvl1_b( "lvl1b");
	TestSettings tset_lvl2("lvl2");
	TestSettings global_tset_lvl2;
	tset_lvl1_a.SetLevel(1);
	tset_lvl2.SetLevel(2);
	tline1->InitPipeline( tset_lvl1_a, metadata, TestPipelineInitializer() );
	tline2->InitPipeline( tset_lvl1_b, metadata, TestPipelineInitializer() );
	tline3->InitPipeline( tset_lvl2, metadata, TestPipelineInitializer() );

	TestPipelineRunnerInstr prunner(false);
	// don't show progress report in this test cases
	prunner.ClearProgressReports();
	prunner.AddPipelines(
			{
				{	tline1, tline2, tline3}});

	TestEventProvider evtProvider;
	// give any pipeline setting here: only the global producer will
	// read from the global settings ...
	prunner.RunPipelines ( evtProvider, global_tset_lvl2 );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	// everything after leve1 is not run on the events,
	// could be changed at a later stage
	tline3->CheckCalls(0,1);
}
