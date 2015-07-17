/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class PtCorrectionProducerLocal: public TraxProducerBase {
public:

	virtual std::string GetProducerId() const override
	{
		return "pt_correction_local";
	}

	virtual void Init(setting_type const& settings)  override
	{
		ProducerBase<TraxTypes>::Init(settings);
	}

	virtual void Produce(TraxEvent const& event,
			TraxProduct & product,
			TraxSettings const& localSettings) const override
	{
		// m_floatPtSim_corrected has been set by the global producer before
		product.m_floatPtSim_corrected = product.m_floatPtSim_corrected
				* localSettings.GetProducerPtCorrectionFactorLocal();
	}

};
