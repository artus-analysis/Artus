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

	virtual bool PopulateGlobalMetaData(TraxEventData const& data,
			TraxGlobalMetaData & metaData,
			TraxGlobalSettings const& globalSettings) const
					ARTUS_CPP11_OVERRIDE {

		metaData.m_floatPtSim_corrected = data.m_floatPtSim
				* globalSettings.GetProducerPtCorrectionFactor();

		return true;
	}

};
