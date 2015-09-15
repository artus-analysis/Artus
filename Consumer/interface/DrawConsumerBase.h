/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

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
	virtual void Init( typename TTypes::setting_type const& settings) override
	{
		ConsumerBase<TTypes>::Init(settings);
	}
};

