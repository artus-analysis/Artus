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
	typedef typename TTypes::global_setting_type global_setting_type;

	KappaEventProviderBase(FileInterface2 & fi, InputTypeEnum inpType) :
			EventProviderBase<TTypes>(),
			m_prevRun(-1), m_prevLumi(-1), m_inpType(inpType), m_fi(fi)
	{
		// setup pointer to collections
		m_event.m_eventMetadata = fi.Get<KEventMetadata>();

		if (inpType == McInput) {
			m_event.m_genEventMetadata = fi.Get<KGenEventMetadata>();
		}
		m_fi.SpeedupTree();

		// auto-delete objects when moving to a new object. Not default root behaviour
		//fi.eventdata.SetAutoDelete(kTRUE);

		m_mon.reset(new ProgressMonitor(GetEntries()));
	}

	/// overwrite and load the Kappa products into your event structure call yourself after 
	/// creating the provider
	virtual void WireEvent( global_setting_type const& ) = 0;

	virtual bool GetEntry(long long lEvent ) {

		if (!m_mon->Update())
			return false;
		m_fi.eventdata.GetEntry(lEvent);

		if (m_prevRun != m_event.m_eventMetadata->nRun) {
			m_prevRun = m_event.m_eventMetadata->nRun;
			m_prevLumi = -1;
		}

		if (m_prevLumi != m_event.m_eventMetadata->nLumi) {
			m_prevLumi = m_event.m_eventMetadata->nLumi;

			// load the correct lumi information
			if (m_inpType == McInput) {
				/*m_event.m_lumimetadata = m_fi.Get<KGenLumiMetadata> (
				 m_event.m_eventMetadata->nRun,
				 m_event.m_eventMetadata->nLumi);*/
			} else if (m_inpType == DataInput) {
				/*m_event.m_lumimetadata = m_fi.Get<KDataLumiMetadata> (
				 m_event.m_eventMetadata->nRun,
				 m_event.m_eventMetadata->nLumi);*/
			} else {
				LOG_FATAL("Unknown input type")
			}

			//std::cout << "Loading new lumi info" << std::endl;
			// reload the HLT information associated with this lumi
			//hltInfo->setLumiMetadata(m_event.m_lumimetadata);
		}

		return true;
	}

	virtual event_type const& GetCurrentEvent() const {
		return m_event;
	}

	virtual long long GetEntries() const {
		return m_fi.eventdata.GetEntries();
	}

protected:

	long m_prevRun, m_prevLumi;
	event_type m_event;

	InputTypeEnum m_inpType;
	boost::scoped_ptr<ProgressMonitor> m_mon;

	FileInterface2 & m_fi;
};

