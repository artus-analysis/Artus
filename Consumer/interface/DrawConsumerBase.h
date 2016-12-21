
#pragma once

#include "Artus/Core/interface/ConsumerBase.h"

/*

Base class for all Consumers which output some form of plot

*/
template<class TTypes>
class DrawConsumerBase: public ConsumerBase<TTypes> {
public:
	DrawConsumerBase() : ConsumerBase<TTypes>() {}
	virtual ~DrawConsumerBase() {}
	void Init( typename TTypes::setting_type const& settings) override
	{
		ConsumerBase<TTypes>::Init(settings);
	}
};

