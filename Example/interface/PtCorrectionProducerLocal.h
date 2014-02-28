/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class PtCorrectionProducerLocal: public TraxLocalProducerBase {
public:

	virtual std::string GetProducerId() {
		return "pt_correction_local";
	}

	virtual void ProduceLocal(TraxEvent const& event,
			TraxProduct & product,
			TraxPipelineSettings const& localSettings) const
					ARTUS_CPP11_OVERRIDE {

		// m_floatPtSim_corrected has been set by the global producer before
		product.m_floatPtSim_corrected = product.m_floatPtSim_corrected
				* localSettings.GetProducerPtCorrectionFactorLocal();

	}

};
