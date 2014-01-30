/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

class TestProduct {
public:
	TestProduct() : iGlobalProduct(0), iLocalProduct(0) {}

	int iGlobalProduct;
	int iLocalProduct;
};
