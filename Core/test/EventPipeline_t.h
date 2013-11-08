/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <vector>

#include <boost/test/included/unit_test.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Artus/Core/interface/EventPipeline.h"

#include "TestGlobalProducer.h"
#include "TestTypes.h"

class TestFilter: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() {
		return "testfilter";
	}

	virtual bool DoesEventPass(const TestData & event,
			TestGlobalMetaData const& metaData, TestSettings const& settings) {
		return (event.iVal < 2);
	}
};

class TestFilter2: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() {
		return "testfilter2";
	}

	virtual bool DoesEventPass(const TestData & event,
			TestGlobalMetaData const& metaData, TestSettings const& settings) {
		return false;
	}
};

class TestLocalMetaDataProducer: public LocalMetaDataProducerBase<TestTypes> {
public:

	// for each pipeline
	virtual void PopulateLocal(TestData const& data,
			TestGlobalMetaData const& metaData,
			TestLocalMetaData & localMetaData,
			TestSettings const& m_pipelineSettings) const {
		localMetaData.iMetaData = data.iVal + 1;
	}
};

class TestEventConsumer: public EventConsumerBase<TestTypes> {
public:
	TestEventConsumer() :
			iFinish(0), iInit(0), iProcessFilteredEvent(0), iProcessEvent(0), iProcess(
					0) {
	}

	virtual void Init(EventPipeline<TestTypes> * pset) ARTUS_CPP11_OVERRIDE {
		m_pipeline = pset;
		iInit++;
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		iFinish++;
	}

	virtual void ProcessFilteredEvent(TestData const& event,
			TestGlobalMetaData const& metaData,
			TestLocalMetaData const& localMetaData) ARTUS_CPP11_OVERRIDE {
		iProcessFilteredEvent++;
	}

	// this method is called for all events
	virtual void ProcessEvent(TestData const& event,
			TestGlobalMetaData const& metaData,
			TestLocalMetaData const& localMetaData, FilterResult & result)
					ARTUS_CPP11_OVERRIDE {
		// did metaData work ?
		BOOST_CHECK_EQUAL(event.iVal + 1, localMetaData.iMetaData);
		BOOST_CHECK_EQUAL(event.iVal + 5 + 23, metaData.iGlobalMetaData);

		iProcessEvent++;
		fres = result;
	}

	virtual void Process() ARTUS_CPP11_OVERRIDE {
		iProcess++;
	}

	void CheckCalls(int ProcessFilteredEvent, int ProcessEvent,
			int Process = 0) {
		BOOST_CHECK_EQUAL(iInit, 1);
		BOOST_CHECK_EQUAL(iFinish, 1);

		BOOST_CHECK_EQUAL(iProcessFilteredEvent, ProcessFilteredEvent);
		BOOST_CHECK_EQUAL(iProcessEvent, ProcessEvent);
		BOOST_CHECK_EQUAL(iProcess, Process);
	}

	int iFinish;
	int iInit;
	int iProcessFilteredEvent;
	int iProcessEvent;
	int iProcess;
	FilterResult fres;
};

class TestPipelineInitilizer: public PipelineInitilizerBase<TestTypes> {
public:
	virtual void InitPipeline(EventPipeline<TestTypes> * pLine,
			TestSettings const& pset) const {
	}

};

BOOST_AUTO_TEST_CASE( test_event_pipeline )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddMetaDataProducer( new TestLocalMetaDataProducer() );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings globa_settings;
	pline.InitPipeline( settings, init );

	TestGlobalProducer globalProducer;
	TestGlobalMetaData global;
	TestData td;
	td.iVal = 23;

	// run global meta data
	globalProducer.PopulateGlobalMetaData( td, global, globa_settings );
	pline.RunEvent( td, global );

	globalProducer.PopulateGlobalMetaData( td, global, globa_settings );
	pline.RunEvent( td, global );

	globalProducer.PopulateGlobalMetaData( td, global, globa_settings );
	pline.RunEvent( td, global );

	pline.FinishPipeline();

	pCons1->CheckCalls(3, 3);
	pCons2->CheckCalls(3, 3);
}

BOOST_AUTO_TEST_CASE( test_add_one_filter2times )
{
	EventPipeline<TestTypes> pline;

	pline.AddFilter( new TestFilter() );
	BOOST_CHECK_THROW( pline.AddFilter( new TestFilter() ),
			std::exception );

}

BOOST_AUTO_TEST_CASE( test_event_filter )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddFilter( new TestFilter() );
	pline.AddMetaDataProducer( new TestLocalMetaDataProducer() );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings global_settings;
	pline.InitPipeline( settings, init );

	TestData td;
	TestGlobalMetaData global;
	TestGlobalProducer globalProducer;

	globalProducer.PopulateGlobalMetaData( td, global, global_settings );
	pline.RunEvent( td, global );
	td.iVal++;
	globalProducer.PopulateGlobalMetaData( td, global, global_settings );
	pline.RunEvent( td, global );
	td.iVal++;
	globalProducer.PopulateGlobalMetaData( td, global, global_settings );
	pline.RunEvent( td, global );

	pline.FinishPipeline();

	pCons1->CheckCalls(2, 3);
	pCons2->CheckCalls(2, 3);
}

BOOST_AUTO_TEST_CASE( test_event_multiplefilter )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();

	TestGlobalProducer globalProducer;
	EventPipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );

	pline.AddFilter( new TestFilter2() );
	pline.AddFilter( new TestFilter() );

	pline.AddMetaDataProducer( new TestLocalMetaDataProducer() );

	TestPipelineInitilizer init;
	TestSettings settings;
	TestGlobalSettings global_settings;
	pline.InitPipeline( settings, init );

	TestData td;
	TestGlobalMetaData global;

	globalProducer.PopulateGlobalMetaData( td, global, global_settings );
	pline.RunEvent( td,global );

	pline.FinishPipeline();

	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter") == true );
	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter2") == false );
	BOOST_CHECK( pCons1->fres.HasPassed() == false);
	/*
	 for ( FilterResult::FilterDecisions::const_iterator it = pCons1->fres.GetFilterDecisions().begin();
	 it != pCons1->fres.GetFilterDecisions().end();
	 it ++ )
	 {
	 if ( it->first == "testfilter" )
	 BOOST_CHECK( it->second == true )

	 if ( it->first == "testfilter" )
	 BOOST_CHECK( it->second == true )
	 std::cout << it->first << " : " << it->second << std::endl;
	 }*/
}

BOOST_AUTO_TEST_CASE( test_event_pipeline_level2 )
{
	TestEventConsumer * pCons1 = new TestEventConsumer();
	TestEventConsumer * pCons2 = new TestEventConsumer();

	EventPipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings global_settings;
	settings.SetLevel ( 2 );
	pline.InitPipeline( settings, init );

	TestGlobalProducer globalProducer;
	TestGlobalMetaData global;
	TestData td;
	td.iVal = 23;

	pline.Run( );
	pline.Run( );
	pline.FinishPipeline();

	pCons1->CheckCalls(0, 0, 2);
	pCons2->CheckCalls(0,0,2 );
}

