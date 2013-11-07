/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include <cassert>

#include <TChain.h>

#include "Artus/Core/interface/EventProviderBase.h"
#include "Artus/Core/interface/GlobalInclude.h"

template<class TEventType>
class RootEventProvider: public EventProviderBase<TEventType> {
public:
	RootEventProvider(stringvector const & fileNames,
			std::string const& treeName) {
		m_rootChain.reset(new TChain(treeName.c_str()));

		for (auto const & fname : fileNames) {
			m_rootChain->Add(fname.c_str());
		}
	}

	virtual bool GotoEvent(long long lEvent) {
		m_rootChain->GetEntry(lEvent);
		return true;
	}

	virtual TEventType const& GetCurrentEvent() const {
		return m_event;
	}

	virtual long long GetOverallEventCount() const {
		return m_rootChain->GetEntries();
	}

protected:

	TEventType m_event;
	boost::scoped_ptr<TChain> m_rootChain;
};

