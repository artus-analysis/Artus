
#pragma once

#include "TestTypes.h"

class TestFilter: public FilterBase<TestTypes> {
public:

	std::string GetFilterId() const override {
		return "testfilter";
	}

	bool DoesEventPass(const TestEvent & event, TestProduct const& product,
	                   TestSettings const& settings, TestMetadata const& metadata) const override
	{
		return (event.iVal < 2);
	}
};

class TestFilter2: public FilterBase<TestTypes> {
public:

	std::string GetFilterId() const override {
		return "testfilter2";
	}

	bool DoesEventPass(const TestEvent & event, TestProduct const& product,
			TestSettings const& settings, TestMetadata const& metadata) const	override
	{
		return false;
	}
};

class TestFilter3: public FilterBase<TestTypes> {
public:

	std::string GetFilterId() const override {
		return "testfilter3";
	}

	bool DoesEventPass(const TestEvent & event, TestProduct const& product,
			TestSettings const& settings, TestMetadata const& metadata) const override
	{
		// this is only 1, if the local producer ran before
		return ( product.iLocalProduct == 1 );
	}
};

class TestGlobalFilter: public FilterBase<TestTypes> {
public:

	std::string GetFilterId() const override {
		return "testglobalfilter";
	}

	bool DoesEventPass(const TestEvent & event, TestProduct const& product,
			TestSettings const& settings, TestMetadata const& metadata) const	override
	{
		return (event.iVal == 0);
	}
};

class TestGlobalFilter2: public FilterBase<TestTypes> {
public:

	std::string GetFilterId() const override {
		return "testglobalfilter2";
	}

	bool DoesEventPass(const TestEvent & event, TestProduct const& product,
			TestSettings const& settings, TestMetadata const& metadata) const	override
	{
		return ( product.iGlobalProduct2 == 1 );
	}
};
