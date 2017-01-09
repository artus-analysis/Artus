
#pragma once

#include <Artus/Core/interface/ProductBase.h>

struct TestProduct : ProductBase {
	TestProduct() : iGlobalProduct(0), iGlobalProduct2(0), iLocalProduct(0) {}

	int iGlobalProduct;
	int iGlobalProduct2;
	int iLocalProduct;
};
