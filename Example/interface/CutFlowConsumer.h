/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/CutFlow.h"

#include "Artus/Consumer/interface/NtupleConsumerBase.h"

#include "TraxTypes.h"


class CutFlowConsumer: public ConsumerBase<TraxTypes> {

public:
	virtual std::string GetConsumerId() const
		ARTUS_CPP11_OVERRIDE
	{
		return "cutflow";
	}

	virtual void Init(Pipeline<TraxTypes> * pipeline) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TraxTypes>::Init( pipeline );

		m_pipelineName = pipeline->GetSettings().GetName ();
	}

	virtual void ProcessEvent(event_type const& event,
			product_type const& product,
            FilterResult & result) {
		m_flow.AddFilterResult ( result );
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {

		std::cout << "Cut Flow for pipeline: " << m_pipelineName << std::endl;
		std::cout << m_flow.ToString() << std::endl;
	}

private:

	CutFlow m_flow;
	std::string m_pipelineName;

};
