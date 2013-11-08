/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class PtCorrectionProducer: public TraxGlobalProducerBase {
public:

	virtual bool PopulateGlobalProduct(TraxEventData const& data,
			TraxGlobalProduct & product,
			TraxGlobalSettings const& globalSettings) const
					ARTUS_CPP11_OVERRIDE {

		product.m_floatPtSim_corrected = data.m_floatPtSim
				* globalSettings.GetProducerPtCorrectionFactor();

		return true;
	}

};
