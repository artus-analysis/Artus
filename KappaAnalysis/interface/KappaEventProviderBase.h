/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <cassert>

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Kappa/DataFormats/interface/KDebug.h"

#include "Artus/Core/interface/PipelineRunner.h"
#include "KappaTools/RootTools/interface/FileInterface2.h"
#include "KappaTools/Toolbox/interface/ProgressMonitor.h"

/**
   \brief Base class to connect the analysis specific event content to the pipelines.

   Defines the basic functionality expected by PipelineRunner. EventProviderBase::WireEvent is a
   purely virtual function that needs to be implemented by any derived class. This function needs
   to be called after the derived EventProvider is instantiated in the main executable.
*/

template<class TTypes>
class KappaEventProviderBase: public EventProviderBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::setting_type setting_type;

	KappaEventProviderBase(FileInterface2 & fi, InputTypeEnum inpType, bool batchMode=false) :
			EventProviderBase<TTypes>(),
			m_prevRun(-1), m_prevLumi(-1), m_prevTree(-1), m_inpType(inpType), m_fi(fi), m_batchMode(batchMode), m_mon(nullptr)
	{
		m_fi.SpeedupTree(128*1024*1024); // in units of bytes

		// auto-delete objects when moving to a new object. Not default root behaviour
		m_fi.eventdata.SetAutoDelete(true);

		m_mon.reset(new ProgressMonitor(GetEntries()));
	}

	/// overwrite and load the Kappa products into your event structure call yourself after 
	/// creating the provider
	virtual void WireEvent(setting_type const& settings)
	{
	}

	bool GetEntry(long long lEvent) override {
		assert(m_event.m_eventInfo);
		assert(m_event.m_lumiInfo);

		if (!m_mon->Update())
			return false;

		long resultGetEntry = m_fi.eventdata.GetEntry(lEvent);
		m_event.m_input = m_fi.eventdata.GetTreeNumber();

		if (m_prevTree != m_fi.eventdata.GetTreeNumber())
		{
			m_prevTree = m_fi.eventdata.GetTreeNumber();
			m_prevLumi = -1;
			LOG(INFO) << "\nProcessing " << m_fi.eventdata.GetFile()->GetName() << " ...";
		}

		if ( NewLumisection()) {
			m_prevRun = m_event.m_eventInfo->nRun;
			m_prevLumi = -1;
		}

		if (NewRun()) {
			m_prevLumi = m_event.m_eventInfo->nLumi;
			m_fi.GetMetaEntry();
		}

		return (resultGetEntry != 0);
	}

	event_type const& GetCurrentEvent() const override {
		return m_event;
	}

	virtual bool NewLumisection() const override { return m_prevLumi != m_event.m_eventInfo->nLumi; }
	virtual bool NewRun() const override {return m_prevRun != m_event.m_eventInfo->nRun; }

	long long GetEntries() const override {
		return (m_batchMode ? m_fi.eventdata.GetEntriesFast() : m_fi.eventdata.GetEntries());
	}


protected:

	long m_prevRun, m_prevLumi;
	int m_prevTree;
	event_type m_event;

	InputTypeEnum m_inpType;

	FileInterface2& m_fi;
	bool m_batchMode;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	template<typename T>
	T* SecureFileInterfaceGet(const std::string &name, const bool check = true, const bool def = false)
	{
		T* result = this->m_fi.template Get<T>(name, check, def);
		if ((GetEntries()) > 0 && (result == nullptr))
		{
			LOG(FATAL) << "Requested branch (" << name << ") not found!";
		}
		return result;
	}

	template<typename T>
	T* SecureFileInterfaceGetMeta(const std::string &name, const bool check = true, const bool def = false) 	{
		T* result = this->m_fi.template GetMeta<T>(name, check, def);
		if ((GetEntries()) > 0 && (result == nullptr))
		{
			LOG(FATAL) << "Requested branch (" << name << ") not found!";
		}
		return result;
	}
};

