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

#include "Artus/Core/interface/Pipeline.h"

#include "TestGlobalProducer.h"
#include "TestLocalProducer.h"
#include "TestConsumer.h"
#include "TestTypes.h"

class TestFilter: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() {
		return "testfilter";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) {
		return (event.iVal < 2);
	}
};

class TestFilter2: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() {
		return "testfilter2";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) {
		return false;
	}
};

class TestPipelineInitilizer: public PipelineInitilizerBase<TestTypes> {
public:
	virtual void InitPipeline(Pipeline<TestTypes> * pLine,
			TestSettings const& pset) const {
	}

};

BOOST_AUTO_TEST_CASE( test_pipeline )
{
	TestConsumer * pCons1 = new TestConsumer();
	TestConsumer * pCons2 = new TestConsumer();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings globalSettings;
	pline.InitPipeline(settings, init);

	TestGlobalProducer globalProducer;
	TestProduct product;
	TestEvent td;
	td.iVal = 23;

	// run global producers
	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);

	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);

	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);

	pline.FinishPipeline();

	pCons1->CheckCalls(3, 3);
	pCons2->CheckCalls(3, 3);
}


BOOST_AUTO_TEST_CASE( test_pipeline_gobal_local_producer )
{
	TestConsumerLocalProduct * pCons1 = new TestConsumerLocalProduct();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddProducer( new TestLocalProducerFromGlobal() );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings globalSettings;
	pline.InitPipeline(settings, init);

	TestGlobalProducer globalProducer;
	TestProduct product;
	TestEvent td;
	td.iVal = 23;

	// run global producers
	globalProducer.ProduceGlobal( td, product, globalSettings );
	pline.RunEvent( td, product );

	pline.FinishPipeline();

    // this value has been computed by the global producer
    // and processed and stored by the local producer    
	pCons1->CheckValue( 52 );
}

BOOST_AUTO_TEST_CASE( test_add_one_filter2times )
{
	Pipeline<TestTypes> pline;

	pline.AddFilter( new TestFilter() );
	BOOST_CHECK_THROW( pline.AddFilter( new TestFilter() ),
			std::exception );

}

BOOST_AUTO_TEST_CASE( test_filter )
{
	TestConsumer * pCons1 = new TestConsumer();
	TestConsumer * pCons2 = new TestConsumer();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddFilter( new TestFilter() );
	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings globalSettings;
	pline.InitPipeline(settings, init);

	TestEvent td;
	TestProduct product;
	TestGlobalProducer globalProducer;

	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);
	td.iVal++;
	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);
	td.iVal++;
	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);

	pline.FinishPipeline();

	pCons1->CheckCalls(2, 3);
	pCons2->CheckCalls(2, 3);
}

BOOST_AUTO_TEST_CASE( test_multiplefilter )
{
	TestConsumer * pCons1 = new TestConsumer();

	TestGlobalProducer globalProducer;
	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );

	pline.AddFilter( new TestFilter2() );
	pline.AddFilter( new TestFilter() );

	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitilizer init;
	TestSettings settings;
	TestGlobalSettings globalSettings;
	pline.InitPipeline( settings, init );

	TestEvent td;
	TestProduct product;

	globalProducer.ProduceGlobal(td, product, globalSettings);
	pline.RunEvent(td, product);

	pline.FinishPipeline();

	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter") == true );
	BOOST_CHECK( pCons1->fres.GetFilterDecisions().at("testfilter2") == false );
	BOOST_CHECK( pCons1->fres.HasPassed() == false);
	/*
	 for (FilterResult::FilterDecisions::const_iterator it = pCons1->fres.GetFilterDecisions().begin();
	 it != pCons1->fres.GetFilterDecisions().end();
	 it ++)
	 {
	 if (it->first == "testfilter")
	 BOOST_CHECK( it->second == true )

	 if (it->first == "testfilter")
	 BOOST_CHECK(it->second == true)
	 std::cout << it->first << " : " << it->second << std::endl;
	 }*/
}

BOOST_AUTO_TEST_CASE( test_event_pipeline_level2 )
{
	TestConsumer * pCons1 = new TestConsumer();
	TestConsumer * pCons2 = new TestConsumer();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	TestPipelineInitilizer init;

	TestSettings settings;
	TestGlobalSettings globalSettings;
	settings.SetLevel(2);
	pline.InitPipeline(settings, init);

	pline.Run();
	pline.Run();
	pline.FinishPipeline();

	pCons1->CheckCalls(0, 0, 2);
	pCons2->CheckCalls(0,0,2);
}

