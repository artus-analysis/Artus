/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

struct TestLocalProduct {
	TestLocalProduct() : iLocalProduct(0) {}

	int iLocalProduct;
};

class TestGlobalProduct {
public:
	TestGlobalProduct() : iGlobalProduct(0) {}
	void ClearContent() {}

	int iGlobalProduct;
};
