/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Pipeline.h"

#include "TestGlobalProducer.h"
#include "TestConsumer.h"
#include "TestTypes.h"

class TestLocalProducer: public ProducerBase<TestTypes> {
public:

	virtual std::string GetProducerId() {
		return "test_local_producer";
	}
	
	// empty to serve as a local producer only
	virtual bool ProduceGlobal(TestEvent const& event,
			TestProduct & globalProduct,
			TestGlobalSettings const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		return true;
	}

	// for each pipeline
	virtual void ProduceLocal(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings) const
            ARTUS_CPP11_OVERRIDE {
		product.iLocalProduct = event.iVal + 1;
	}
};


class TestLocalProducerFromGlobal: public ProducerBase<TestTypes> {
public:

	virtual std::string GetProducerId() {
		return "test_local_global_producer";
	}
	
	// empty to serve as a local producer only
	virtual bool ProduceGlobal(TestEvent const& event,
			TestProduct & globalProduct,
			TestGlobalSettings const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		return true;
	}

	// for each pipeline
	virtual void ProduceLocal(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings) const
            ARTUS_CPP11_OVERRIDE {
		product.iLocalProduct = product.iGlobalProduct + 1;
	}
};



