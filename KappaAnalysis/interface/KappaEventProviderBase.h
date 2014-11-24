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
#include "KappaTools/RootTools/FileInterface2.h"
#include "KappaTools/Toolbox/ProgressMonitor.h"

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

	KappaEventProviderBase(FileInterface2 & fi, InputTypeEnum inpType) :
			EventProviderBase<TTypes>(),
			m_prevRun(-1), m_prevLumi(-1), m_prevTree(-1), m_inpType(inpType), m_fi(fi)
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

	virtual bool GetEntry(long long lEvent ) ARTUS_CPP11_OVERRIDE {
		assert(m_event.m_eventMetadata);
		assert(m_event.m_lumiMetadata);

		if (!m_mon->Update())
		{
			return false;
		}
		
		m_fi.eventdata.GetEntry(lEvent);
		
		if (m_prevTree != m_fi.eventdata.GetTreeNumber()) {
			m_prevTree = m_fi.eventdata.GetTreeNumber();
			LOG(INFO) << "\nProcessing " << m_fi.eventdata.GetFile()->GetName() << " ...";
		}

		if (m_prevRun != m_event.m_eventMetadata->nRun) {
			m_prevRun = m_event.m_eventMetadata->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventMetadata->nLumi) {
			m_prevLumi = m_event.m_eventMetadata->nLumi;
			m_fi.GetMetaEntry();
		}

		return true;
	}

	virtual event_type const& GetCurrentEvent() const ARTUS_CPP11_OVERRIDE {
		return m_event;
	}

	virtual long long GetEntries() const ARTUS_CPP11_OVERRIDE {
		return m_fi.eventdata.GetEntries();
	}


protected:

	long m_prevRun, m_prevLumi;
	int m_prevTree;
	event_type m_event;

	InputTypeEnum m_inpType;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	FileInterface2 & m_fi;

	template<typename T>
	T* SecureFileInterfaceGet(const std::string &name, const bool check = true, const bool def = false)
	{
		T* result = this->m_fi.template Get<T>(name, check, def);
		if(result == 0)
			LOG(FATAL) << "Requested branch (" << name << ") not found!";
		return result;
	}
	
	template<typename T>
	T* SecureFileInterfaceGetMeta(const std::string &name, const bool check = true, const bool def = false) 	{
		T* result = this->m_fi.template GetMeta<T>(name, check, def);
		if(result == 0)
			LOG(FATAL) << "Requested branch (" << name << ") not found!";
		return result;
	}
};

