#pragma once

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingMacros.h"

class SettingsBase {
public:

	// path in the config file to reach the settings for this pipeline
	IMPL_PROPERTY(std::string, SettingsRoot)
	// pointer to the global, loaded property tree
	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)

	// default setting content
	IMPL_SETTING(size_t, Level)

	virtual std::string ToString() const {
		return "SettingsBase - ToString";
	}

};
