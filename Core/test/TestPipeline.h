/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

class TestPipeline: public EventPipeline<TestTypes> {
public:

	TestPipeline() :
			iRunEvent(0), iRun(0), iFinish(0), bCheckProducer(false) {

	}

	virtual void RunEvent(TestData const& evt, TestGlobalProduct const& product) ARTUS_CPP11_OVERRIDE {
		iRunEvent++;
		//if (bCheckProducer) {
		//BOOST_CHECK(product.iGlobalProduct == 1);
		//}
	}

	virtual void Run() ARTUS_CPP11_OVERRIDE {
		iRun ++;
	}

	virtual void FinishPipeline() ARTUS_CPP11_OVERRIDE {
		iFinish++;
	}

	void CheckCalls(int RunEvent, int Run = 0) {
		BOOST_CHECK_EQUAL(iFinish, 1);
		BOOST_CHECK_EQUAL(iRunEvent, RunEvent);
		BOOST_CHECK_EQUAL(iRun, Run);
	}

	int iRunEvent;
	int iRun;
	int iFinish;
	bool bCheckProducer;
};
