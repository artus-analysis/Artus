
#pragma once

#include "Artus/Core/interface/ConsumerBase.h"

/*

Base class for all Consumers which output some form of plot

*/
template<class TTypes>
class DrawConsumerBase: public ConsumerBase<TTypes> {
public:
	
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;
	
	DrawConsumerBase() : ConsumerBase<TTypes>() {}
	virtual ~DrawConsumerBase() {}
	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		ConsumerBase<TTypes>::Init(settings, metadata);
	}
};

