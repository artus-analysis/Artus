#pragma once

class TestPipeline: public EventPipeline<TestTypes> {
public:

	TestPipeline() :
			iRunEvent(0), iRun(0), iFinish(0), bCheckMetaProducer(false) {

	}

	virtual void RunEvent(TestData const& evt, TestMetaData const& meta) ARTUS_CPP11_OVERRIDE {
		iRunEvent++;
		//if (bCheckMetaProducer) {
		//BOOST_CHECK(meta.iGlobalMetaData == 1);
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
	bool bCheckMetaProducer;
};
