
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
			TestSettings const& globalSettings,
			TestMetadata const& metadata) const override {
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
			TestSettings const& globalSettings,
			TestMetadata const& metadata) const override {
		product.iGlobalProduct2 = event.iVal + 1;
	}
};
