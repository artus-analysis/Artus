
#pragma once

#include "Artus/Core/interface/Pipeline.h"

#include "TestGlobalProducer.h"
#include "TestConsumer.h"
#include "TestTypes.h"

class TestLocalProducer: public ProducerBase<TestTypes> {
public:

	std::string GetProducerId() const override {
		return "test_local_producer";
	}

	// for each pipeline
	void Produce(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings,
			TestMetadata const& metadata) const override
	{
		product.iLocalProduct = event.iVal + 1;
	}
};


class TestLocalProducerFromGlobal: public ProducerBase<TestTypes> {
public:

	std::string GetProducerId() const override
	{
		return "test_local_global_producer";
	}

	// for each pipeline
	void Produce(TestEvent const& event,
			TestProduct & product,
			TestSettings const& m_pipelineSettings,
			TestMetadata const& metadata) const override
	{
		product.iLocalProduct = product.iGlobalProduct + 1;
	}
};



