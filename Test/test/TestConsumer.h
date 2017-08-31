
#pragma once

#include "Artus/Core/interface/Pipeline.h"

class TestConsumer: public ConsumerBase<TestTypes> {
public:
	explicit TestConsumer(bool checkInProcessEvent = true) : bCheckInProcessEvent(checkInProcessEvent), iFinish(0), iInit(0),
		iProcessFilteredEvent(0), iProcessEvent(0), iProcess(0) {
	}

	std::string GetConsumerId() const override {
		return "test_consumer";
	}

	void Init(TestSettings const& setting, TestMetadata& metadata) override {
		++iInit;
	}

	void Finish(TestSettings const& setting, TestMetadata const& metadata) override {
		++iFinish;
	}

	void ProcessFilteredEvent(TestEvent const& event, TestProduct const& product,
			TestSettings const& setting, TestMetadata const& metadata) override
	{
		++iProcessFilteredEvent;
	}

	// this method is called for all events
	void ProcessEvent(TestEvent const& event, TestProduct const& product,
			TestSettings const& setting, TestMetadata const& metadata,
			FilterResult& result) override
	{
		if ( bCheckInProcessEvent ) {
			// did product work ?
			BOOST_CHECK_EQUAL(event.iVal + 1, product.iLocalProduct);
			BOOST_CHECK_EQUAL(event.iVal + 5 + 23, product.iGlobalProduct);
		}

		++iProcessEvent;
		fres = result;
	}

	void Process(TestSettings const& setting, TestMetadata const& metadata) override {
		++iProcess;
	}

	void CheckCalls(int ProcessFilteredEvt, int ProcessEvt,	int Process_ = 0) {
		BOOST_CHECK_EQUAL(iInit, 1);
		BOOST_CHECK_EQUAL(iFinish, 1);

		BOOST_CHECK_EQUAL(iProcessFilteredEvent, ProcessFilteredEvt);
		BOOST_CHECK_EQUAL(iProcessEvent, ProcessEvt);
		BOOST_CHECK_EQUAL(iProcess, Process_);
	}

	bool bCheckInProcessEvent;
	int iFinish;
	int iInit;
	int iProcessFilteredEvent;
	int iProcessEvent;
	int iProcess;
	FilterResult fres;
};


class TestConsumerLocalProduct: public ConsumerBase<TestTypes> {
public:
	TestConsumerLocalProduct() : m_iLocalValue (0) {
	}

	std::string GetConsumerId() const override {
		return "test_consumer_local";
	}

	void ProcessFilteredEvent(TestEvent const& event, TestProduct const& product,
			TestSettings const& setting, TestMetadata const& metadata) override
	{
		m_iLocalValue = product.iLocalProduct;
	}

	void Finish (TestSettings const& setting, TestMetadata const& metadata) override {}

	void CheckValue(int compareTo ) {
		BOOST_CHECK_EQUAL( m_iLocalValue, compareTo );
	}

	int m_iLocalValue;
};


