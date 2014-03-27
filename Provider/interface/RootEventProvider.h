/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */
#pragma once

#include <cassert>

#include <TChain.h>

#include "Artus/Core/interface/EventProviderBase.h"
#include "Artus/Core/interface/GlobalInclude.h"

template<class TTypes>
class RootEventProvider: public EventProviderBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;

	RootEventProvider(stringvector const & fileNames,
			std::string const& treeName) {
		m_rootChain.reset(new TChain(treeName.c_str()));

		for (auto const & fname : fileNames) {
			m_rootChain->Add(fname.c_str());
		}
	}

	virtual bool GetEntry(long long lEvent) {
		m_rootChain->GetEntry(lEvent);
		return true;
	}

	virtual event_type const& GetCurrentEvent() const {
		return m_event;
	}

	virtual long long GetEntries() const {
		return m_rootChain->GetEntries();
	}

	virtual void WireEvent( TraxTypes::global_setting_type const& ) = 0;


protected:

	event_type m_event;
	boost::scoped_ptr<TChain> m_rootChain;
};

