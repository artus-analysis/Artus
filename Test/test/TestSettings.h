/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Configuration/interface/SettingsBase.h"

class TestSettings : public SettingsBase {
public:

	TestSettings() : SettingsBase(), m_Level( 1) {
	}

	TestSettings( std::string lineName ) : SettingsBase( lineName ), m_Level( 1) {

	}

	std::string ToString() const {
		return "Test setting";
	}

	IMPL_PROPERTY(unsigned int, Level)

	// needs to be overwritte here, because the test
	// cases don't have a json file loaded and
	// the code would fail if a lookup to the json file
	// would happen
	virtual stringvector GetFilters () const
		ARTUS_CPP11_OVERRIDE
	{
		return stringvector();
	}
};

class TestGlobalSettings : public GlobalSettingsBase {
public:

	TestGlobalSettings() :
			m_Offset(23) {

	}

	IMPL_PROPERTY(unsigned int, Offset)

	// needs to be overwritte here, because the test
	// cases don't have a json file loaded and
	// the code would fail if a lookup to the json file
	// would happen
	virtual stringvector GetGlobalFilters () const
		ARTUS_CPP11_OVERRIDE
	{
		return stringvector();
	}
};
