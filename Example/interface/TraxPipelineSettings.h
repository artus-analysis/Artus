/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

class TraxPipelineSettings: public SettingsBase {
public:

	IMPL_SETTING( float, FilterPtLow )
	IMPL_SETTING( float, FilterPtHigh )

	IMPL_SETTING( float, ProducerPtCorrectionFactorLocal )

	VarCache<stringvector> quantities;
	stringvector GetQuantities() const
	{
		RETURN_CACHED(quantities, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Pipelines." + GetName() + ".Quantities"))
	}

};

class TraxGlobalSettings: public GlobalSettingsBase {
public:

	IMPL_SETTING( float, ProducerPtCorrectionFactor )

};
