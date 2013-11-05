#pragma once

#include "Artus/Core/interface/EventConsumerBase.h"

template<class TTypes>
class DrawConsumerBase: public EventConsumerBase<TTypes> {
public:
	DrawConsumerBase() :
			EventConsumerBase<TTypes>() {
	}
	virtual ~DrawConsumerBase() {
	}
};
