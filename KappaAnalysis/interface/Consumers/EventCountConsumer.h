
#pragma once

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "TH1.h"

class EventCountConsumer: public ConsumerBase<KappaTypes>
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	EventCountConsumer();
	
	virtual std::string GetConsumerId() const override;
	virtual void ProcessEvent(event_type const& event, product_type const& product, setting_type const& settings, FilterResult & result) override;
	
	virtual void Finish(setting_type const& settings);
	unsigned int currentLumi;

protected:
	TH1I* m_totalEvents;
	TH1I* m_filteredEvents;
};

