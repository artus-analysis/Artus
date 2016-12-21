
#pragma once

#include <cassert>

#include <boost/scoped_ptr.hpp>

#include <TChain.h>

#include "Artus/Core/interface/EventProviderBase.h"

template<class TTypes>
class RootEventProvider: public EventProviderBase<TTypes> {

public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	RootEventProvider(std::vector<std::string> const& fileNames,
			std::string const& treeName) {
		m_rootChain.reset(new TChain(treeName.c_str()));

		for (auto const& fname : fileNames) {
			m_rootChain->Add(fname.c_str());
		}
	}

	bool GetEntry(long long lEvent) override {
		return (m_rootChain->GetEntry(lEvent) != 0);
	}

	event_type const& GetCurrentEvent() const override{
		return m_event;
	}

	long long GetEntries() const override {
		return m_rootChain->GetEntries();
	}

	virtual void WireEvent(setting_type const& settings) = 0;

protected:
	event_type m_event;
	boost::scoped_ptr<TChain> m_rootChain;
};

