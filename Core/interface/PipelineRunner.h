/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <thread>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <map>
#include <sys/time.h>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "Pipeline.h"
#include "EventProviderBase.h"
#include "ProgressReport.h"
#include "FilterResult.h"
#include "OsSignalHandler.h"

/**
 \brief Class to manage all registered Pipelines and to connect them to the event.

 The PipelineRunner utilizes a user-provided EventProvider to load events and passes them to all
 registered Pipelines. The EventProvider is passed to the function PipelineRunner::RunPipelines
 as an argument. Furthermore, Producers can be registered, which can generate Pipeline-
 independet products of the event. These Producers are run before any pipeline is started
 and the generated data is passed on to the pipelines.
 */
template<typename TPipeline, typename TTypes>
class PipelineRunner: public boost::noncopyable
{
public:

	// set registerSignalHandlern = true if you want to register
	// a signal listener with the OS to handle Ctrl-C commands send
	// by the user to the console. In this case the processing will be stopped
	// after the current event and the root file will be properly written.
	PipelineRunner(bool registerSignalHandler = true) : m_registerSignalHandler(registerSignalHandler)
	{
		// this is the default
		// use AddProgressReport / ClearProgressReports to adapt it to your needs
		AddProgressReport(new ConsoleProgressReport());

		// register on signals to allow for a graceful shutdown if the user
		// requests it
		if (m_registerSignalHandler)
		{
			osRegisterHandler();
		}
	}

	~PipelineRunner() {
		if (m_registerSignalHandler)
			osUnregisterHandler();
	}

	typedef TPipeline pipeline_type;

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef ProducerBaseUntemplated producer_base_type;
	typedef FilterBaseUntemplated filter_base_type;

	typedef boost::ptr_list<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	typedef boost::ptr_list<ProcessNodeBase> ProcessNodes;
	typedef typename ProcessNodes::iterator ProcessNodesIterator;

	typedef boost::ptr_list<ProgressReportBase> ProgressReportList;
	typedef typename ProgressReportList::iterator ProgressReportIterator;

	/// Add a pipeline. The object is destroyed in the destructor of the PipelineRunner.
	void AddPipeline(TPipeline* pline)
	{
		m_pipelines.push_back(pline);
	}

	/// Add a filter. The object is destroyed in the destructor of the PipelineRunner.
	// the execution order of AddGlobalFilter and AddGlobalProducer is defined by the order
	// this methods are called
	void AddFilter(filter_base_type* filter)
	{
		m_globalNodes.push_back(filter);
	}

	/// Add a GlobalProducer. The object is destroyed in the destructor of the PipelineRunner.
	// the execution order of AddGlobalFilter and AddGlobalProducer is defined by the order
	// this methods are called
	void AddProducer(producer_base_type* prod)
	{
		m_globalNodes.push_back(prod);
	}

	/// Add a range of pipelines. The object is destroyed in the destructor of the PipelineRunner.
	void AddPipelines(std::vector<TPipeline*> pVec)
	{

		for (auto * it : pVec)
		{
			AddPipeline(it);
		}
	}

	/// Run the Producers and all pipelines. Give any pipeline setting here: only the
	/// producer will read from the settings ...
	template<class TEventProvider>
	void RunPipelines(TEventProvider & evtProvider,
			setting_type const& settings)
	{
		long long firstEvent = settings.GetFirstEvent();
		long long nEvents = evtProvider.GetEntries();
		long long processNEvents = settings.GetProcessNEvents();
		if (processNEvents > 0)
		{
			nEvents = processNEvents;
		}
		const stringvector globlalFilterIds = settings.GetFilters();
		const stringvector taggingFilters = settings.GetTaggingFilters();

		// initialize pline filter decision
		FilterResult::FilterNames pipelineResultNames(m_pipelines.size());
		std::transform(m_pipelines.begin(), m_pipelines.end(),
				pipelineResultNames.begin(),
				[] ( pipeline_type const& p ) -> std::string
				{	return p.GetSettings().GetName();});

		{
			auto pipelineResultNamesSorted = pipelineResultNames;
			// must be sorted to perform the unique option
			// but don't use the sorted strings later on
			std::sort(pipelineResultNamesSorted.begin(),
					pipelineResultNamesSorted.end());

			auto itUnq = std::unique(pipelineResultNamesSorted.begin(),
					pipelineResultNamesSorted.end());
			if (itUnq != pipelineResultNamesSorted.end())
			{
				LOG(FATAL)<< "Pipeline name '" << *itUnq << "' is not unique, but pipeline names must be unique";
			}
		}
		for (long long i = firstEvent; ( (processNEvents<0) || (i<(firstEvent + nEvents)) ); ++i)
		{

			// quit here according to OS
			if (osHasSIGINT())
			{
				LOG(INFO)<< "Terminating processing due to received SIGTERM";
				break;
			}

			if (!evtProvider.GetEntry(i))
			break;
			for (ProgressReportIterator it = m_progressReport.begin();
					it != m_progressReport.end(); it++)
			{
				it->update(i-firstEvent, nEvents);
			}

			product_type productGlobal;
			// use the lit of filters to bootstrap the filter list names
			FilterResult globalFilterResult ( globlalFilterIds, taggingFilters );

			for( ProcessNodesIterator it = m_globalNodes.begin();
					it != m_globalNodes.end(); it ++ )
			{
				// variables for runtime measurement
				timeval tStart, tEnd;
				int runTime;

				// stop processing as soon as one filter fails
				// but the consumers will still be processed
				if (! globalFilterResult.HasPassed())
				break;

				if ( it->GetProcessNodeType () == ProcessNodeType::Producer )
				{
					producer_base_type& prod = static_cast<producer_base_type&>(*it);
					//LOG(DEBUG) << prod.GetProducerId() << "::Produce";
					gettimeofday(&tStart, nullptr);
					ProducerBaseAccess(prod).Produce(evtProvider.GetCurrentEvent(),
							productGlobal, settings);
					gettimeofday(&tEnd, nullptr);
					runTime = static_cast<int>(tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec);
					productGlobal.processorRunTime[prod.GetProducerId()] = runTime;
				}
				else if ( it->GetProcessNodeType () == ProcessNodeType::Filter )
				{
					filter_base_type& flt = static_cast<filter_base_type&>(*it);
					//LOG(DEBUG) << flt.GetFilterId() << "::DoesEventPass";
					gettimeofday(&tStart, nullptr);
					const bool filterResult = FilterBaseAccess(flt).DoesEventPass(evtProvider.GetCurrentEvent(),
							productGlobal, settings);
					globalFilterResult.SetFilterDecision(flt.GetFilterId(), filterResult);
					gettimeofday(&tEnd, nullptr);
					runTime = static_cast<int>(tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec);
					productGlobal.processorRunTime[flt.GetFilterId()] = runTime;
				}
				else
				{
					LOG(FATAL) << "ProcessNodeType not supported by the pipeline runner!";
				}
			}

			// run the pipelines
			FilterResult pipelineFilterRes(pipelineResultNames, taggingFilters);

			for (PipelinesIterator it = m_pipelines.begin();
					it != m_pipelines.end(); it++)
			{
				if (it->GetSettings().GetLevel() == 1)
				{
					productGlobal.PreviousPipelinesResult = pipelineFilterRes;
					bool result = it->RunEvent(evtProvider.GetCurrentEvent(),
							productGlobal, globalFilterResult);
					pipelineFilterRes.SetFilterDecision(
							it->GetSettings().GetName(), result);
				}
			}
		}

		for (ProgressReportIterator it = m_progressReport.begin();
				it != m_progressReport.end(); it++)
		{
			it->finish();
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator it = m_pipelines.begin();
				!(it == m_pipelines.end()); it++)
		{
			if (it->GetSettings().GetLevel() == 1)
				it->FinishPipeline();
		}

		osSignalReset();

		// run the pipelines greater level one
		// even if the previous event loop was interrupted
		bool noPipelineRun = false;
		size_t curLevel = 2;
		while (!noPipelineRun)
		{
			noPipelineRun = true;
			for (PipelinesIterator it = m_pipelines.begin();
					it != m_pipelines.end(); it++)
			{
				if (osHasSIGINT())
				{
					LOG(INFO)<< "Terminating processing due to received SIGTERM";
					break;
				}

				if (it->GetSettings().GetLevel() == curLevel)
				{
					noPipelineRun = false;

					it->Run();
					it->FinishPipeline();
				}
			}
			curLevel++;
		}
	}

	void AddProgressReport(ProgressReportBase * p)
	{
		m_progressReport.push_back(p);
	}

	void ClearProgressReports()
	{
		m_progressReport.clear();
	}

	Pipelines & GetPipelines()
	{
		return m_pipelines;
	}

	ProcessNodes & GetNodes()
	{
		return m_globalNodes;
	}

private:

	Pipelines m_pipelines;
	ProcessNodes m_globalNodes;
	ProgressReportList m_progressReport;
	bool m_registerSignalHandler;
};

