/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Provider/interface/RootEventProvider.h"

#include "TraxTypes.h"

/*
 * Will load the corresponding ntuple from a root file
 * The memory locations are passed to ROOT one time, in the
 * WireEvent() method call.
 */

class TraxEventProvider: public RootEventProvider<TraxTypes> {
public:
	explicit TraxEventProvider(std::vector<std::string> const& fileNames);

	virtual void WireEvent(TraxSettings const& settings) override;
};
