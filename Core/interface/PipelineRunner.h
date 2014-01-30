/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "Pipeline.h"
#include "EventProviderBase.h"
#include "ProgressReport.h"

/*
 * The PipelineRunner utilizes a user-provided EventProvider to load events and passes them to
 * all registered Pipelines.
 * Furthermore, GlobalProducers can be registered, which can generate Pipeline-independet products
 * of the event. These GlobalProducers are run before any pipeline is started and the generated data is passed to the
 * pipelines.
 */

template<typename TPipeline, typename TGlobalProducer>
class PipelineRunner: public boost::noncopyable {
public:

	PipelineRunner() {
		// this is the default
		// use AddProgressReport / ClearProgressReports to adapt it to your needs
		AddProgressReport(new ConsoleProgressReport());
	}

	typedef TPipeline pipeline_type;

	typedef boost::ptr_list<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	typedef boost::ptr_list<TGlobalProducer> GlobalProducer;
	typedef typename GlobalProducer::iterator GlobalProducerIterator;

	typedef boost::ptr_list<ProgressReportBase> ProgressReportList;
	typedef typename ProgressReportList::iterator ProgressReportIterator;

	/*
	 * Add a pipeline. The object is destroy in the destructor of the PipelineRunner
	 */
	void AddPipeline(TPipeline* pline) {
		m_pipelines.push_back(pline);
	}

	/*
	 * Add a GlobalProducer. The object is destroy in the destructor of the PipelineRunner
	 */
	void AddGlobalProducer(TGlobalProducer* prod) {
		m_globalProducer.push_back(prod);
	}

	/*
	 * Add a range of pipelines. The object is destroy in the destructor of the PipelineRunner
	 */
	void AddPipelines(std::vector<TPipeline*> pVec) {

		for (auto * it : pVec) {
			AddPipeline(it);
		}
	}

	/*
	 * Run the GlobalProducers and all pipelines.
	 * give any pipeline setting here: only the global producer will
	 * read from the global settings ...
	 *
	 */
	template<class TTypes>
	void RunPipelines(
			EventProviderBase<typename TTypes::event_type>& evtProvider,
			typename TTypes::global_setting_type const& globalSettings) {
		long long firstEvent = 0; // settings.Global()->GetSkipEvents();
		long long nEvents = evtProvider.GetEntries();
		/*if (settings.Global()->GetEventCount() >= 0)
		 nEvents = firstEvent + settings.Global()->GetEventCount();
		 if (firstEvent != 0 || nEvents != evtProvider.GetEntries())
		 LOG(red << "Warning: Custom range of events: " << firstEvent << " to " << nEvents << reset)
		 */
		bool bEventValid = true;

		// init global producers
		for (GlobalProducerIterator it = m_globalProducer.begin();
				it != m_globalProducer.end(); it++) {
			it->Init(globalSettings);
		}

		for (long long i = firstEvent; i < nEvents; ++i) {

			for (ProgressReportIterator it = m_progressReport.begin();
					it != m_progressReport.end(); it++) {
				it->update(i, nEvents);
			}
			if (!evtProvider.GetEntry(i))
				break;

			typename TTypes::product_type productGlobal;
			//productGlobal.m_hltInfo = hltTools;

			// create global products
			for (GlobalProducerIterator it = m_globalProducer.begin();
					it != m_globalProducer.end(); it++) {
				bEventValid = it->ProduceGlobal(evtProvider.GetCurrentEvent(),
						productGlobal, globalSettings);
				//LOG(it->GetContent())
				if (!bEventValid)
					break;
			}

			// run the pipelines, if the event is valid
			if (bEventValid) {
				for (PipelinesIterator it = m_pipelines.begin();
						it != m_pipelines.end(); it++) {
					if (it->GetSettings().GetLevel() == 1)
						it->RunEvent(evtProvider.GetCurrentEvent(),
								productGlobal);
				}
			}
		}

		for (ProgressReportIterator it = m_progressReport.begin();
				it != m_progressReport.end(); it++) {
			it->finish(nEvents - 1, nEvents);
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator it = m_pipelines.begin();
				!(it == m_pipelines.end()); it++) {
			if (it->GetSettings().GetLevel() == 1)
				it->FinishPipeline();
		}

		// run the pipelines greater level one
		bool noPipelineRun = false;
		size_t curLevel = 2;
		while (!noPipelineRun) {
			noPipelineRun = true;
			for (PipelinesIterator it = m_pipelines.begin();
					it != m_pipelines.end(); it++) {
				if (it->GetSettings().GetLevel() == curLevel) {
					noPipelineRun = false;

					it->Run();
					it->FinishPipeline();
				}
			}
			curLevel++;
		}
	}

	void AddProgressReport(ProgressReportBase * p) {
		m_progressReport.push_back(p);
	}

	void ClearProgressReports() {
		m_progressReport.clear();
	}

private:

	Pipelines m_pipelines;
	GlobalProducer m_globalProducer;
	ProgressReportList m_progressReport;
};

