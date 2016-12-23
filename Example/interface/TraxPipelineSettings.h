
#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

/*
class TraxPipelineSettings: public SettingsBase {
public:


};
*/
class TraxSettings: public SettingsBase {
public:

	IMPL_SETTING( float, FilterThetaLow )
	IMPL_SETTING( float, FilterThetaHigh )

	IMPL_SETTING( float, ProducerPtCorrectionFactor )

	IMPL_SETTING( float, FilterPtLow )
	IMPL_SETTING( float, FilterPtHigh )

	IMPL_SETTING( float, ProducerPtCorrectionFactorLocal )

	IMPL_SETTING_STRINGLIST( Quantities )

};
