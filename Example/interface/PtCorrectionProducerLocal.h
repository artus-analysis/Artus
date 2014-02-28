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

		product.m_floatPtSim_corrected = event.m_floatPtSim
				* localSettings.GetProducerPtCorrectionFactorLocal();

	}

};
