/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/ProducerBase.h"

#include "TestTypes.h"

typedef ProducerBase<TestTypes> TestProducerBase;

class TestGlobalProducer: public TestProducerBase {
public:
	std::string GetProducerId() const override {
		return "test_global_producer";
	}
	
	void Produce(TestEvent const& event,
			TestProduct & product,
			TestSettings const& globalSettings) const override {
		product.iGlobalProduct = event.iVal + 5 + globalSettings.GetOffset();
	}
};

class TestGlobalProducer2: public TestProducerBase {
public:
	std::string GetProducerId() const override {
		return "test_global_producer2";
	}

	void Produce(TestEvent const& event,
			TestProduct & product,
			TestSettings const& globalSettings) const override {
		product.iGlobalProduct2 = event.iVal + 1;
	}
};
