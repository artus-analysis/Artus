#pragma once

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"

class TraxPipelineSettings: public SettingsBase {
public:


	IMPL_SETTING( float, FilterPtLow )
	IMPL_SETTING( float, FilterPtHigh )

};
