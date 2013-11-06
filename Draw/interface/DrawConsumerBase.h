#pragma once

#include "Artus/Core/interface/EventConsumerBase.h"

/*

Base class for all Consumers which output some form of plot

*/

template<class TTypes>
class DrawConsumerBase: public EventConsumerBase<TTypes> {
public:
	DrawConsumerBase() :
			EventConsumerBase<TTypes>() {
	}
	virtual ~DrawConsumerBase() {
	}
};
