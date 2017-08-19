
#pragma once

#include "Artus/Core/interface/CutFlow.h"
#include "Artus/Core/interface/Pipeline.h"

template < class TTypes >
class CutFlowConsumerBase: public ConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	std::string GetConsumerId() const override
	{
		return "cutflow";
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		ConsumerBase<TTypes>::Init(settings, metadata);

		m_pipelineName = settings.GetName();
	}

	void ProcessEvent(event_type const& event, product_type const& product,
	                  setting_type const& settings, metadata_type const& metadata,
	                  FilterResult & result) override
	{
		ConsumerBase<TTypes>::ProcessEvent(event, product, settings, metadata, result);
		m_flow.AddFilterResult(result);
	}

	void Finish(setting_type const& settings, metadata_type const& metadata) override
	{
		// at this point, m_flow contains all the filter results of all events
		// overwrite this to analyze
	}

protected:

	CutFlow m_flow;
	std::string m_pipelineName;
};
