
#pragma once

#include <cassert>
#include <chrono>
#include <future>

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
		// RF: Deactivated since it caused trouble when running on multiple files
		//m_fi.eventdata->SetAutoDelete(true);

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
		
		// lood entries asynchronously and exit the program, if looding the entry takes unreasonably long (dCache, ...)
		TChain* eventdata = m_fi.eventdata;
		std::future<long> futureGetEntry = std::async(std::launch::async, [eventdata, lEvent] () -> long
		{
			return eventdata->GetEntry(lEvent);
		});
		std::chrono::minutes timeout(5);
		if (futureGetEntry.wait_for(timeout) != std::future_status::ready)
		{
			LOG(FATAL) << "Timeout: Could not read entry from Events tree!";
		}
		long resultGetEntry = futureGetEntry.get();
		
		m_event.m_input = eventdata->GetTreeNumber();

		if (m_prevTree != eventdata->GetTreeNumber())
		{
			m_prevTree = eventdata->GetTreeNumber();
			m_prevLumi = -1;
			LOG(INFO) << "\nProcessing " << eventdata->GetFile()->GetName() << " ...";
		}

		if (  m_prevRun != m_event.m_eventInfo->nRun ) {
			m_prevRun = m_event.m_eventInfo->nRun;
			m_prevLumi = -1;
			m_newRun = true;
		}
		else
			m_newRun = false;

		if ( m_prevLumi != m_event.m_eventInfo->nLumi ) {
			m_prevLumi = m_event.m_eventInfo->nLumi;
			
			// lood entries asynchronously and exit the program, if looding the entry takes unreasonably long (dCache, ...)
			FileInterface2* fi = &m_fi;
			std::future<void> futureGetLumiEntry = std::async(std::launch::async, [fi] ()
			{
				fi->GetLumiEntry();
			});
			if (futureGetLumiEntry.wait_for(timeout) != std::future_status::ready)
			{
				LOG(FATAL) << "Timeout: Could not read entry from Lumis tree!";
			}
			
			m_newLumisection = true;
		}
		else
			m_newLumisection = false;

		return (resultGetEntry != 0);
	}

	event_type const& GetCurrentEvent() const override {
		return m_event;
	}

	virtual bool NewLumisection() const override { return m_newLumisection; }
	virtual bool NewRun() const override { return m_newRun; }

	long long GetEntries() const override {
		return (m_batchMode ? m_fi.eventdata->GetEntriesFast() : m_fi.eventdata->GetEntries());
	}


protected:
	bool m_newLumisection, m_newRun;
	long m_prevRun, m_prevLumi;
	int m_prevTree;
	event_type m_event;

	InputTypeEnum m_inpType;

	FileInterface2& m_fi;
	bool m_batchMode;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	template<typename T>
	T* SecureFileInterfaceGetEvent(const std::string &name, const bool check = true, const bool def = false)
	{
		T* result = nullptr;
		if (GetEntries() > 0)
		{
			result = this->m_fi.template GetEvent<T>(name, check, def);
			if (result == nullptr)
			{
				LOG(FATAL) << "Requested branch (" << name << ") not found!";
			}
		}
		return result;
	}

	template<typename T>
	T* SecureFileInterfaceGetLumi(const std::string &name, const bool check = true, const bool def = false)
	{
		T* result = nullptr;
		if (GetEntries() > 0)
		{
			result = this->m_fi.template GetLumi<T>(name, check, def);
			if (result == nullptr)
			{
				LOG(FATAL) << "Requested branch (" << name << ") not found!";
			}
		}
		return result;
	}

	template<typename T>
	T* SecureFileInterfaceGetRun(const std::string &name, const bool check = true, const bool def = false)
	{
		T* result = nullptr;
		if (GetEntries() > 0)
		{
			result = this->m_fi.template GetRun<T>(name, check, def);
			if (result == nullptr)
			{
				LOG(FATAL) << "Requested branch (" << name << ") not found!";
			}
		}
		return result;
	}
};

