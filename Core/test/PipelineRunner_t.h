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
#include "TestFactory.h"

#include <boost/test/included/unit_test.hpp>

class TestGlobalFilter: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "testglobalfilter";
	}

	virtual bool DoesEventPassGlobal(const TestEvent & event,
			TestProduct const& product, TestGlobalSettings const& settings) const
	ARTUS_CPP11_OVERRIDE
	{
		return (event.iVal == 0);
	}
};

class TestGlobalFilter2: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "testglobalfilter2";
	}

	virtual bool DoesEventPassGlobal(const TestEvent & event,
			TestProduct const& product, TestGlobalSettings const& settings) const
	ARTUS_CPP11_OVERRIDE
	{
		return ( product.iGlobalProduct2 == 1 );
	}
};

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

	PipelineRunner<TestPipeline, TestTypes > prunner;
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
	prunner.RunPipelines ( evtProvider, global_tset );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	tline3->CheckCalls(10);
	tline4->CheckCalls(10);
	tline5->CheckCalls(10);
}


BOOST_AUTO_TEST_CASE( test_event_prunner_global_producer_filter )
{
	TestPipeline * tline1 = new TestPipeline;

	tline1->bCheckProducer = true;

	TestSettings tset;
	TestGlobalSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );

	PipelineRunner<TestPipeline, TestTypes > prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );

	prunner.AddGlobalProducer( new TestGlobalProducer() );
	// this filter will drop all events
	prunner.AddGlobalFilter( new TestGlobalFilter2() );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	// pipeline should never have been called
	tline1->CheckCalls(0);
}

BOOST_AUTO_TEST_CASE( test_event_prunner_global_producer_filter_pass )
{
	TestPipeline * tline1 = new TestPipeline;

	tline1->bCheckProducer = true;

	TestSettings tset;
	TestGlobalSettings global_tset;
	tset.SetLevel(1);
	tline1->InitPipeline( tset, TestPipelineInitilizer() );

	PipelineRunner<TestPipeline, TestTypes > prunner;
	// don't show progress report in this test cases
	prunner.ClearProgressReports();

	prunner.AddPipeline( tline1 );

	prunner.AddGlobalProducer( new TestGlobalProducer() );
	prunner.AddGlobalFilter( new TestGlobalFilter() );
	// this filter will drop all events
	prunner.AddGlobalProducer( new TestGlobalProducer2() );
	prunner.AddGlobalFilter( new TestGlobalFilter2() );

	TestEventProvider evtProvider;
	prunner.RunPipelines ( evtProvider, global_tset );

	// pipeline should never have been called
	tline1->CheckCalls(10);
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
	prunner.RunPipelines ( evtProvider, global_tset );

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
	prunner.RunPipelines ( evtProvider, global_tset_lvl2 );

	tline1->CheckCalls(10);
	tline2->CheckCalls(10);
	// everything after leve1 is not run on the events,
	// could be changed at a later stage
	tline3->CheckCalls(0,1);
}
