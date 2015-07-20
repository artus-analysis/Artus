/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

class PtCorrectionProducer: public TraxProducerBase
{
public:

	virtual std::string GetProducerId() const override
	{
		return "pt_correction";
	}

	virtual void Init(TraxSettings const& globalSettings) override
	{
		ProducerBase<TraxTypes>::Init(globalSettings);
	}

	virtual void Produce(TraxEvent const& event, TraxProduct & product,
			TraxSettings const& globalSettings) const override
	{

		product.m_floatPtSim_corrected = event.m_floatPtSim
				* globalSettings.GetProducerPtCorrectionFactor();
	}

};
