
#pragma once

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


class PrintHltConsumer: public ConsumerBase<KappaTypes>
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	PrintHltConsumer();
	
	std::string GetConsumerId() const override;
	
	void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) override;
	
	virtual void Finish(setting_type const& settings);

};

