
#pragma once

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

class KappaPipelineSettings: public SettingsBase {
public:

};

class KappaGlobalSettings: public GlobalSettingsBase {

public:
	IMPL_SETTING_DEFAULT(std::string, Electrons, "");
	IMPL_SETTING_DEFAULT(std::string, Muons, "");
	IMPL_SETTING_DEFAULT(std::string, Taus, "");
	
	IMPL_SETTING_DEFAULT(std::string, Jets, "");
	IMPL_SETTING_DEFAULT(std::string, JetArea, "");
	
	IMPL_SETTING_DEFAULT(std::string, Met, "");
	
	IMPL_SETTING_DEFAULT(std::string, BeamSpot, "");
	IMPL_SETTING_DEFAULT(std::string, VertexSummary, "");
	
	IMPL_SETTING_DEFAULT(std::string, GenParticles, "");
	
	IMPL_SETTING_DEFAULT(std::string, LumiMetadata, ""); // TODO: Default value, move to Artus/Provider
	IMPL_SETTING_DEFAULT(std::string, GenLumiMetadata, ""); // TODO: Default value, move to Artus/Provider
	IMPL_SETTING_DEFAULT(std::string, EventMetadata, ""); // TODO: Default value, move to Artus/Provider
	IMPL_SETTING_DEFAULT(std::string, GenEventMetadata, ""); // TODO: Default value, move to Artus/Provider
	IMPL_SETTING_DEFAULT(std::string, FilterMetadata, ""); // TODO: Default value, move to Artus/Provider
	IMPL_SETTING_DEFAULT(std::string, FilterSummary, ""); // TODO: Default value, move to Artus/Provider

};
