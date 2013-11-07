/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

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
	
	virtual ~SettingsBase() {
	}

	// path in the config file to reach the settings for this pipeline
	IMPL_PROPERTY(std::string, PropTreePath)
	// pointer to the global, loaded property tree
	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)

	// pipeline leve, the default if no entry is in the json file will be 1
	IMPL_SETTING_DEFAULT(size_t, Level, 1 )

	IMPL_PROPERTY( std::string, Name )

	// the folder name in the output root file where plots or ntuples of
	// this pipeline will end up,
	// if you want it not to be the pipeline name, override it
	virtual std::string GetRootFileFolder() const {
		return GetName();
	}
 
	// a pointer to the root file where all the output will be stored
	// must be set by the application
	IMPL_PROPERTY(TFile *, RootOutFile)

	virtual std::string ToString() const {
		return "SettingsBase - Pipeline name: " + GetName();
	}

};
