/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Provider/interface/RootEventProvider.h"

#include "TraxTypes.h"

/*
 * Will load the corresponding ntuple from a root file
 * The memory locations are passed to ROOT one time, in the
 * WireEvent() method call.
 */

class TraxEventProvider: public RootEventProvider<TraxTypes> {
public:
	TraxEventProvider(stringvector const & fileNames) :
			RootEventProvider<TraxTypes>(fileNames,
			// hardcode the root treename already here
					"ec") {
	}

	virtual void WireEvent( TraxTypes::global_setting_type const& )
		ARTUS_CPP11_OVERRIDE
	{
		// set up the ROOT pointers to our local memory regions
		m_rootChain->SetBranchAddress("theSim", &m_event.m_floatTheSim);
		m_rootChain->SetBranchAddress("pSim", &m_event.m_floatPSim);
		m_rootChain->SetBranchAddress("ptSim", &m_event.m_floatPtSim);
		m_rootChain->SetBranchAddress("pzSim", &m_event.m_floatPzSim);
	}
};
