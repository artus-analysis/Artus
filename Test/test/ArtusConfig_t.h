/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <sstream>
#include <boost/test/included/unit_test.hpp>

#include "Artus/Configuration/interface/ArtusConfig.h"

#include "TestTypes.h"
#include "TestPipelineRunner.h"

BOOST_AUTO_TEST_CASE( test_parse_config )
{
	std::stringstream configStream;

	configStream
	<<	"{"
	<<	    "\"GlobalFilterProducer\": ["
	<<	    "    \"filter:testglobalfilter\", \"producer:test_global_producer\""
	<<	    "],"
	<<	    "\"InputFiles\": ["
	<<	    "    \"sample_ntuple.root\""
	<<	    "],"
	<<	    "\"OutputPath\": \"sample_output.root\","
	<<	    "\"Pipelines\": {"
	<<	    "    \"lowPt\": {"
	<<	    "        \"Consumers\": [ \"test_consumer\""
	<<	    "        ],"
	<<	    "       \"FilterProducer\": [\"producer:test_local_producer\", \"filter:testfilter\", \"filter:testfilter2\" "
	<<	    "        ]"
	<<	    "    },"
	<<	    "    \"highPt\": {"
	<<	    "        \"Consumers\": [ \"test_consumer\""
	<<	    "        ],"
	<<	    "       \"FilterProducer\": [ \"producer:test_local_producer\", \"filter:testfilter2\""
	<<	    "        ]"
	<<	    "    }"
	<<	    "}"
	<<	"}";
	std::cout << std::endl << configStream.str();

	ArtusConfig cfg ( configStream );

	TestEventProvider evtProvider;

	// the pipeline initializer will setup the pipeline, with
	// all the attached Producer, Filer and Consumer
	TestPipelineInitializer pInit;
	TestFactory factory;
	TestPipelineRunner runner;

	// load the pipeline with their configuration from the config file
	cfg.LoadConfiguration( pInit, runner, factory, ARTUS_CPP11_NULLPTR);

	auto & pList = runner.GetGlobalNodes();
	//auto itFirst = pList.begin();

	BOOST_CHECK_EQUAL( pList.size(), size_t(2) );
	BOOST_CHECK( pList.begin()->GetProcessNodeType() == ProcessNodeType::Filter );
	//BOOST_CHECK( (pList.begin() + 1 )->GetProcessNodeType() == ProcessNodeType::Filter );

	auto & pLine = runner.GetPipelines();

	BOOST_CHECK_EQUAL( pLine.size() , size_t(2) );

	auto pLineOne = pLine.begin();

	BOOST_CHECK_EQUAL( pLineOne->GetNodes().size() , size_t(3) );

	auto & nodesOne = pLineOne->GetNodes();

	BOOST_CHECK( nodesOne.begin()->GetProcessNodeType() == ProcessNodeType::Producer );
/*	BOOST_CHECK( (nodesOne.begin() + 1)->GetProcessNodeType() == ProcessNodeType::Filter );
	BOOST_CHECK( (nodesOne.begin() + 2)->GetProcessNodeType() == ProcessNodeType::Producer );
*/

	//BOOST_CHECK_EQUAL( runner.GetGlobalNodes().at(1).GetProcessNodeType(), ProcessNodeType::Filter );

}

