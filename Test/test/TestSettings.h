/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"

class TestSettings : public SettingsBase {
public:

	TestSettings() :
			m_Level(1) {

	}

	std::string ToString() const {
		return "Test setting";
	}

	IMPL_PROPERTY(unsigned int, Level)

};

class TestGlobalSettings : public GlobalSettingsBase {
public:

	TestGlobalSettings() :
			m_Offset(23) {

	}

	IMPL_PROPERTY(unsigned int, Offset)

};
