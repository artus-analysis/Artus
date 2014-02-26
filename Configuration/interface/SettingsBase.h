/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TFile.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Configuration/interface/SettingMacros.h"

/**
   \brief Reads setting of local parts of PipelineRunner from a prepared json configuration file.

   Defines what is needed to read configurations for Artus event processing from a prepared json 
   file and passes it on to LocalProducers, Filters and Consumers.
*/

class SettingsBase {
public:

	SettingsBase() :
			m_RootOutFile(ARTUS_CPP11_NULLPTR) {
	}

	virtual ~SettingsBase() {
	}

	/// path in the config file to reach the settings for this pipeline
	IMPL_PROPERTY(std::string, PropTreePath)
	/// pointer to the global, loaded property tree
	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)

	/// pipeline level, the default if no entry is in the json file will be 1
	IMPL_SETTING_DEFAULT(size_t, Level, 1 )

	IMPL_PROPERTY( std::string, Name )

	/// the folder name in the output root file where plots or ntuples of this pipeline will end 
	/// up, if you want it not to be the pipeline name, override it
	virtual std::string GetRootFileFolder() const {
		return GetName();
	}

	/// a pointer to the root file where all the output will be stored must be set by the 
	/// application
	IMPL_PROPERTY(TFile *, RootOutFile)

	virtual std::string ToString() const {
		return "SettingsBase - Pipeline name: " + GetName();
	}

	/// get list of all filters
	VarCache<stringvector> m_filters;
	stringvector GetFilters() const
	{
		RETURN_CACHED(m_filters, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Filters"))
	}

	/// get list of all local producers
	VarCache<stringvector> m_localProducers;
	stringvector GetLocalProducers() const
	{
		RETURN_CACHED(m_localProducers, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".LocalProducers"))
	}

	/// get list of all consumers
	VarCache<stringvector> m_consumers;
	stringvector GetConsumers() const
	{
		RETURN_CACHED(m_consumers, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Consumers"))
	}

};

/**
   \brief Reads settings for global parts of PipelineRunner from a prepared json configuration file.

   Defines what is needed to read configurations for Artus event processing from a prepared json 
   file and passes it on to GlobalProducers.
*/

class GlobalSettingsBase {
public:
	/// path in the config file to reach the settings for this pipeline
	IMPL_PROPERTY(std::string, PropTreePath)
	/// pointer to the global, loaded property tree
	IMPL_PROPERTY(boost::property_tree::ptree*, PropTree)

	/// get list of all local producers
	VarCache<stringvector> m_globalProducers;
	stringvector GetGlobalProducers() const
	{
		RETURN_CACHED(m_globalProducers, PropertyTreeSupport::GetAsStringList(GetPropTree(), "GlobalProducers"))
	}

};
