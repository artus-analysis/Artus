/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "TestTypes.h"

class TestFilter: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const override {
		return "testfilter";
	}

	virtual bool DoesEventPass(const TestEvent & event, TestProduct const& product,
	                           TestSettings const& settings) const override
	{
		return (event.iVal < 2);
	}
};

class TestFilter2: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const override {
		return "testfilter2";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) const	override
	{
		return false;
	}
};

class TestFilter3: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const override {
		return "testfilter3";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) const override
	{
		// this is only 1, if the local producer ran before
		return ( product.iLocalProduct == 1 );
	}
};

class TestGlobalFilter: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const override {
		return "testglobalfilter";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) const	override
	{
		return (event.iVal == 0);
	}
};

class TestGlobalFilter2: public FilterBase<TestTypes> {
public:

	virtual std::string GetFilterId() const override {
		return "testglobalfilter2";
	}

	virtual bool DoesEventPass(const TestEvent & event,
			TestProduct const& product, TestSettings const& settings) const	override
	{
		return ( product.iGlobalProduct2 == 1 );
	}
};
