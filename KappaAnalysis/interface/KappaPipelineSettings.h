
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
	IMPL_SETTING_DEFAULT(std::string, Muons, "");
	IMPL_SETTING_DEFAULT(std::string, Jets, "");
	IMPL_SETTING_DEFAULT(std::string, BeamSpot, "");
	IMPL_SETTING_DEFAULT(std::string, VertexSummary, "");
	IMPL_SETTING_DEFAULT(std::string, LumiMetadata, ""); // TODO

};
