
#pragma once

#include <vector>

#include <boost/test/included/unit_test.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Artus/Core/interface/Pipeline.h"

#include "TestGlobalProducer.h"
#include "TestPipelineRunner.h"
#include "TestLocalProducer.h"
#include "TestConsumer.h"
#include "TestFilter.h"
#include "TestTypes.h"

BOOST_AUTO_TEST_CASE( test_pipeline )
{
	TestConsumer * pCons1 = new TestConsumer();
	TestConsumer * pCons2 = new TestConsumer();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitializer init;

	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	pline.InitPipeline(settings, metadata, init);

	TestGlobalProducer globalProducer;
	TestProduct product;
	TestEvent td;
	td.iVal = 23;
	FilterResult globalFilterResult;

	// run global producers
	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

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

	TestPipelineInitializer init;

	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	pline.InitPipeline(settings, metadata, init);

	TestGlobalProducer globalProducer;
	TestProduct product;
	TestEvent td;
	td.iVal = 23;
	FilterResult globalFilterResult;

	// run global producers
	globalProducer.Produce( td, product, globalSettings, metadata );
	pline.RunEvent( td, product, globalFilterResult );

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

	pline.AddProducer( new TestLocalProducer() );
	pline.AddFilter( new TestFilter() );

	TestPipelineInitializer init;

	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	pline.InitPipeline(settings, metadata, init);

	TestEvent td;
	TestProduct product;
	TestGlobalProducer globalProducer;
	FilterResult globalFilterResult;

	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);
	td.iVal++;
	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);
	td.iVal++;
	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

	pline.FinishPipeline();

	pCons1->CheckCalls(2, 3);
	pCons2->CheckCalls(2, 3);
}

BOOST_AUTO_TEST_CASE( test_multiplefilter_producer )
{
	TestConsumer * pCons1 = new TestConsumer();

	TestGlobalProducer globalProducer;
	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );

	pline.AddFilter( new TestFilter() );
	pline.AddProducer( new TestLocalProducer() );
	pline.AddFilter( new TestFilter3() );
	// if run after TestFilter3, TestFilter3 will be still true
	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitializer init;
	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	pline.InitPipeline(settings, metadata, init);

	TestEvent td;
	TestProduct product;
	FilterResult globalFilterResult;

	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

	pline.FinishPipeline();

	BOOST_CHECK( pCons1->fres.GetFilterDecision("testfilter") == FilterResult::Decision::Passed );
	BOOST_CHECK( pCons1->fres.GetFilterDecision("testfilter3") == FilterResult::Decision::Passed );
	BOOST_CHECK( pCons1->fres.HasPassed() == true);
}

BOOST_AUTO_TEST_CASE( test_multiplefilter_producer_stop_exec )
{
	TestConsumer * pCons1 = new TestConsumer( false );

	TestGlobalProducer globalProducer;
	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );

	// this first filter will stop the further
	// processing of filters & producers right away
	pline.AddFilter( new TestFilter3() );
	pline.AddFilter( new TestFilter2() );
	pline.AddFilter( new TestFilter() );
	pline.AddProducer( new TestLocalProducer() );

	TestPipelineInitializer init;
	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	pline.InitPipeline(settings, metadata, init);

	TestEvent td;
	TestProduct product;
	FilterResult globalFilterResult;

	globalProducer.Produce(td, product, globalSettings, metadata);
	pline.RunEvent(td, product, globalFilterResult);

	pline.FinishPipeline();

	BOOST_CHECK( pCons1->fres.GetFilterDecisions().size() == 1 );
	BOOST_CHECK( pCons1->fres.GetFilterDecision("testfilter3") == FilterResult::Decision::NotPassed );
	BOOST_CHECK( pCons1->fres.HasPassed() == false);
}

BOOST_AUTO_TEST_CASE( test_event_pipeline_level2 )
{
	TestConsumer * pCons1 = new TestConsumer();
	TestConsumer * pCons2 = new TestConsumer();

	Pipeline<TestTypes> pline;

	pline.AddConsumer( pCons1 );
	pline.AddConsumer( pCons2 );

	TestPipelineInitializer init;

	TestMetadata metadata;
	TestSettings settings;
	TestSettings globalSettings;
	settings.SetLevel(2);
	pline.InitPipeline(settings, metadata, init);

	pline.Run();
	pline.Run();
	pline.FinishPipeline();

	pCons1->CheckCalls(0, 0, 2);
	pCons2->CheckCalls(0,0,2);
}

