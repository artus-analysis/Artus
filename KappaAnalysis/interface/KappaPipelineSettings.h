
#pragma once

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

/**
   \brief Reads settings for local parts of KappaPipelineRunner from a prepared json configuration 
   file. 

   Defines local settings that can be obtained from the json configuration file. These settings are
   then available as key value pairs of type:

   KappaGlobalSetting mysetting;
   mysetting.key = value

   for an implementation of type:

   IMPL_SETTING_DEFAULT(type_of_value, key, default_value);

   You can define the readout of new global settings here or in a derived class.
*/

class KappaPipelineSettings: public SettingsBase {
public:

};

/**
   \brief Reads settings for global parts of KappaPipelineRunner from a prepared json configuration 
   file. 

   Defines global settings that can be obtained from the json configuration file. These settings are
   then available as key value pairs of type:

   KappaGlobalSetting mysetting;
   mysetting.key = value

   for an implementation of type:

   IMPL_SETTING_DEFAULT(type_of_value, key, default_value);

   You can define the readout of new global settings here or in a derived class.
*/

class KappaGlobalSettings: public GlobalSettingsBase {

public:
        /// name of electron collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Electrons, "");
        /// name of muon collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Muons, "");
        /// name of tau collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Taus, "");
        /// name of jet collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Jets, "");
        /// name of jet area collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, JetArea, "");
	/// name of MET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Met, "");
	/// name of beamspot collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, BeamSpot, "");
        /// name of vertexSummary collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, VertexSummary, "");
	/// name of genParticle collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenParticles, "");
	/// name of lumiMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, LumiMetadata, "");     // TODO: Default value, move to Artus/Provider
	/// name of genLumiMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenLumiMetadata, "");  // TODO: Default value, move to Artus/Provider
	/// name of eventMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, EventMetadata, "");    // TODO: Default value, move to Artus/Provider
	/// name of genEventMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenEventMetadata, ""); // TODO: Default value, move to Artus/Provider
	/// name of filterMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterMetadata, "");   // TODO: Default value, move to Artus/Provider
	/// name of filterSummary in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterSummary, "");    // TODO: Default value, move to Artus/Provider
};
