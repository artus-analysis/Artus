#pragma once

struct TestLocalMetaData {
	TestLocalMetaData() :
			iMetaData(0) {
	}

	int iMetaData;
};

class TestGlobalMetaData {
public:
	TestGlobalMetaData() :
			iGlobalMetaData(0) {
	}

	void ClearContent() {
	}

	int iGlobalMetaData;
};
