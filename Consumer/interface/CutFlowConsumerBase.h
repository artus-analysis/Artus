/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/CutFlow.h"
#include "Artus/Core/interface/Pipeline.h"

template < class TTypes >
class CutFlowConsumerBase: public ConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetConsumerId() const
		ARTUS_CPP11_OVERRIDE
	{
		return "cutflow";
	}

	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::Init( pipeline );

		m_pipelineName = pipeline->GetSettings().GetName ();
	}

	virtual void ProcessEvent(event_type const& event,
			product_type const& product,
            FilterResult & result) {
		ConsumerBase<TTypes>::ProcessEvent(event, product, result);
		m_flow.AddFilterResult ( result );
	}

	virtual void Finish() ARTUS_CPP11_OVERRIDE {
		// at this point, m_flow contains all the filter results of all events
		// overwrite this to analyze
	}

protected:

	CutFlow m_flow;
	std::string m_pipelineName;
};
