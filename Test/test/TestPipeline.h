
#pragma once

#include <functional>

#include "TestTypes.h"

class TestPipelineInstr: public Pipeline<TestTypes> {
public:

	TestPipelineInstr() : iRunEvent(0), iRun(0), iFinish(0), bCheckProducer(false), bFullyRun(true) {
		lmdRunEventCheck = [](TestProduct const& globalProduct ){};
	}

	bool RunEvent( TestEvent const& event,
                           TestProduct const& globalProduct,
                           FilterResult const& globalFilterResult) override {
		++iRunEvent;
		//if (bCheckProducer) {
		//BOOST_CHECK(globalProduct.iGlobalProduct == 1);
		//}
		lmdRunEventCheck( globalProduct );

		return bFullyRun;
	}

	void Run() override {
		++iRun;
	}

	void FinishPipeline() override {
		++iFinish;
	}

	void CheckCalls(int RunEvt, int Run_ = 0) {
		BOOST_CHECK_EQUAL(iFinish, 1);
		BOOST_CHECK_EQUAL(iRunEvent, RunEvt);
		BOOST_CHECK_EQUAL(iRun, Run_);
	}

	int iRunEvent;
	int iRun;
	int iFinish;
	bool bCheckProducer;
	bool bFullyRun;
	std::function< void ( TestProduct const& globalProduct ) > lmdRunEventCheck;
};

typedef Pipeline<TestTypes> TestPipeline;
