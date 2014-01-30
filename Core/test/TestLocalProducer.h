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

class TestLocalProducer: public LocalProducerBase<TestTypes> {
public:

	// for each pipeline
	virtual void ProduceLocal(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings) const
            ARTUS_CPP11_OVERRIDE {
		product.iLocalProduct = event.iVal + 1;
	}
};


class TestLocalProducerFromGlobal: public LocalProducerBase<TestTypes> {
public:

	// for each pipeline
	virtual void ProduceLocal(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings) const
            ARTUS_CPP11_OVERRIDE {
		product.iLocalProduct = product.iGlobalProduct + 1;
	}
};



