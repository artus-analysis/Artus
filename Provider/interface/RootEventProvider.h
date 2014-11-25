/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */
#pragma once

#include <cassert>

#include <TChain.h>

#include "Artus/Core/interface/EventProviderBase.h"

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
		return (m_rootChain->GetEntry(lEvent) != 0);
	}

	virtual event_type const& GetCurrentEvent() const {
		return m_event;
	}

	virtual long long GetEntries() const {
		return m_rootChain->GetEntries();
	}

	virtual void WireEvent( TraxSettings const& ) = 0;


protected:

	event_type m_event;
	boost::scoped_ptr<TChain> m_rootChain;
};

