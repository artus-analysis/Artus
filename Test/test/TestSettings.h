
#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"

class TestSettings : public SettingsBase {
public:

	TestSettings() : SettingsBase(), m_Level( 1), m_Offset(23) {
	}

	explicit TestSettings(std::string lineName) : SettingsBase(lineName), m_Level(1) {
	}

	std::string ToString() const {
		return "Test setting";
	}

	IMPL_PROPERTY(unsigned int, Level)

	// needs to be overwritte here, because the test
	// cases don't have a json file loaded and
	// the code would fail if a lookup to the json file
	// would happen
	std::vector<std::string> GetFilters () const override
	{
		return std::vector<std::string>();
	}
	std::vector<std::string> & GetTaggingFilters () const override
	{
		return m_taggingFilters;
	}
	mutable std::vector<std::string> m_taggingFilters;

	long long GetProcessNEvents () const
	{
		return -1;
	}

	long long GetFirstEvent () const
	{
		return 0;
	}

	IMPL_PROPERTY(unsigned int, Offset)
};

