/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TestTypes.h"

class TestPipelineInstr: public Pipeline<TestTypes> {
public:

	TestPipelineInstr() : iRunEvent(0), iRun(0), iFinish(0), bCheckProducer(false) {

	}

	virtual void RunEvent( TestEvent const& event, 
                           TestProduct const& globalProduct,
                           FilterResult const& globalFilterResult) ARTUS_CPP11_OVERRIDE {
		iRunEvent++;
		//if (bCheckProducer) {
		//BOOST_CHECK(globalProduct.iGlobalProduct == 1);
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

typedef Pipeline<TestTypes> TestPipeline;
