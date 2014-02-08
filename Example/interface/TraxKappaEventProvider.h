/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Provider/interface/KappaEventProvider.h"

#include "TraxTypes.h"

/*
 * Will load the corresponding ntuple from a root file
 * The memory locations are passed to ROOT one time, in the
 * WireEvent() method call.
 */

class TraxKappaEventProvider: public KappaEventProvider<TraxTypes> {
public:
	TraxKappaEventProvider(FileInterface2 & fi, InputTypeEnum inpType) :
		KappaEventProvider<TraxTypes>(fi, inpType) {
	}

	virtual void WireEvent( TraxTypes::global_setting_type const& )
		ARTUS_CPP11_OVERRIDE
	{
		// set up the Kappa pointers to our local memory regions
	}
};
