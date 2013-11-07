/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

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
