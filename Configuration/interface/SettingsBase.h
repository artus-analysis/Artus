#pragma once

#include "TFile.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingMacros.h"

class SettingsBase {
public:

	SettingsBase() :
			m_RootOutFile(ARTUS_CPP11_NULLPTR) {
	}

	// path in the config file to reach the settings for this pipeline
IMPL_PROPERTY(std::string, PropTreePath)
	// pointer to the global, loaded property tree
IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)

	// default setting content
IMPL_SETTING(size_t, Level)

	// the folder name in the output root file where plots or ntuples of
	// this pipeline will end up,
	// is read from JSON config file
IMPL_SETTING(std::string, RootFileFolder)

	// a pointer to the root file where all the output will be stored
	// must be set by the application
IMPL_PROPERTY(TFile *, RootOutFile)

	virtual std::string ToString() const {
		return "SettingsBase - ToString";
	}

};
