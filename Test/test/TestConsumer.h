/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Pipeline.h"

class TestConsumer: public ConsumerBase<TestTypes> {
public:
	TestConsumer( bool checkInProcessEvent = true) : bCheckInProcessEvent(checkInProcessEvent), iFinish(0), iInit(0),
		iProcessFilteredEvent(0), iProcessEvent(0), iProcess(0) {
	}

	virtual std::string GetConsumerId() const override {
		return "test_consumer";
	}

	virtual void Init(TestSettings const& setting) override {
		//m_pipeline = pset;
		iInit++;
	}

	virtual void Finish(TestSettings const& setting) override {
		iFinish++;
	}

	virtual void ProcessFilteredEvent(TestEvent const& event,
			TestProduct const& product,
			TestSettings const& setting) override
	{
		iProcessFilteredEvent++;
	}

	// this method is called for all events
	virtual void ProcessEvent(TestEvent const& event,
			TestProduct const& product,
			TestSettings const& setting,
			FilterResult& result) override
	{
		if ( bCheckInProcessEvent ) {
			// did product work ?
			BOOST_CHECK_EQUAL(event.iVal + 1, product.iLocalProduct);
			BOOST_CHECK_EQUAL(event.iVal + 5 + 23, product.iGlobalProduct);
		}

		iProcessEvent++;
		fres = result;
	}

	virtual void Process(TestSettings const& setting) override {
		iProcess++;
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

	virtual std::string GetConsumerId() const override {
		return "test_consumer_local";
	}

	virtual void ProcessFilteredEvent(TestEvent const& event,
			TestProduct const& product,
			TestSettings const& setting) override
	{
		m_iLocalValue = product.iLocalProduct;
	}

	virtual void Finish ( TestSettings const& setting ) {

	}

	void CheckValue(int compareTo ) {
		BOOST_CHECK_EQUAL( m_iLocalValue, compareTo );
	}

	int m_iLocalValue;
};


