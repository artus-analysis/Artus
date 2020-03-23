
#pragma once

#include <thread>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <map>
#include <sys/time.h>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "Artus/Core/interface/Pipeline.h"
#include "Artus/Core/interface/EventProviderBase.h"
#include "Artus/Core/interface/ProgressReport.h"
#include "Artus/Core/interface/FilterResult.h"
#include "Artus/Core/interface/OsSignalHandler.h"
#include "Artus/Core/interface/MetadataBase.h"

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
	explicit PipelineRunner(bool registerSignalHandler = true) : m_registerSignalHandler(registerSignalHandler)
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

	~PipelineRunner()
	{
		if (m_registerSignalHandler)
		{
			osUnregisterHandler();
		}
	}

	typedef TPipeline pipeline_type;

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

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
		for (TPipeline* pipeline : pVec)
		{
			AddPipeline(pipeline);
		}
	}

	/// Run the Producers and all pipelines. Give any pipeline setting here: only the
	/// producer will read from the settings ...
	template<class TEventProvider>
	void RunPipelines(TEventProvider& evtProvider, setting_type const& settings)
	{
		long long firstEvent = settings.GetFirstEvent();
		long long nEvents = evtProvider.GetEntries();
		long long processNEvents = settings.GetProcessNEvents();
		if (processNEvents > 0)
		{
			nEvents = processNEvents;
		}
		const std::vector<std::string> globlalFilterIds = settings.GetFilters();
		const std::vector<std::string> taggingFilters = settings.GetTaggingFilters();

		// initialize pline filter decision
		FilterResult::FilterNames pipelineResultNames(m_pipelines.size());
		std::transform(m_pipelines.begin(), m_pipelines.end(), pipelineResultNames.begin(),
		               [](pipeline_type const& p) -> std::string { return p.GetSettings().GetName(); });

		std::vector<std::string> pipelineResultNamesSorted = pipelineResultNames;
		// must be sorted to perform the unique option
		// but don't use the sorted strings later on
		std::sort(pipelineResultNamesSorted.begin(), pipelineResultNamesSorted.end());
		std::vector<std::string>::iterator itUnq = std::unique(pipelineResultNamesSorted.begin(), pipelineResultNamesSorted.end());
		if (itUnq != pipelineResultNamesSorted.end())
		{
			LOG(FATAL)<< "Pipeline name '" << *itUnq << "' is not unique, but pipeline names must be unique";
		}

		// apparently evtProvider.GetEntries() is not reliable. Therefore, if 'ProcessNEvents' is not set (=-1), the loop condition
		// always evaluates to true (processNEvents<0) = (-1<0) and is terminated via the 'if (!evtProvider.GetEntry(i)) break' statement
		for (long long iEvent = firstEvent; (iEvent < (firstEvent + nEvents)); ++iEvent)
		{
			// quit here according to OS
			if (osHasSIGINT())
			{
				LOG(INFO)<< "Terminating processing due to received SIGTERM";
				break;
			}

			if (!evtProvider.GetEntry(iEvent))
			{
				break;
			}

			for (ProgressReportIterator report = m_progressReport.begin(); report != m_progressReport.end(); ++report)
			{
				report->update(iEvent-firstEvent, nEvents);
			}

			product_type productGlobal;
			// use the lit of filters to bootstrap the filter list names
			FilterResult globalFilterResult(globlalFilterIds, taggingFilters);

			for (ProcessNodesIterator processNode = m_globalNodes.begin(); processNode != m_globalNodes.end(); ++processNode)
			{
				// variables for runtime measurement
				timeval tStart, tEnd;
				int runTime;

				// stop processing as soon as one filter fails
				// but the consumers will still be processed
				if (! globalFilterResult.HasPassed())
				{
					break;
				}

				event_type currentEvent = evtProvider.GetCurrentEvent();
				productGlobal.newRun = evtProvider.NewRun();
				productGlobal.newLumisection = evtProvider.NewLumisection();

				if (processNode->GetProcessNodeType() == ProcessNodeType::Producer)
				{
					producer_base_type& prod = static_cast<producer_base_type&>(*processNode);
					gettimeofday(&tStart, nullptr);

					if (productGlobal.newRun)
					{
						ProducerBaseAccess(prod).OnRun(currentEvent, settings, m_globalMetadata);
					}
					if (productGlobal.newLumisection)
					{
						ProducerBaseAccess(prod).OnLumi(currentEvent, settings, m_globalMetadata);
					}
					ProducerBaseAccess(prod).Produce(currentEvent, productGlobal, settings, m_globalMetadata);

					gettimeofday(&tEnd, nullptr);
					runTime = static_cast<int>(tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec);
					productGlobal.processorRunTime[prod.GetProducerId()] = runTime;
				}
				else if ( processNode->GetProcessNodeType () == ProcessNodeType::Filter )
				{
					filter_base_type& flt = static_cast<filter_base_type&>(*processNode);
					gettimeofday(&tStart, nullptr);

					if (productGlobal.newRun)
					{
						FilterBaseAccess(flt).OnRun(currentEvent, settings, m_globalMetadata);
					}
					if (productGlobal.newLumisection)
					{
						FilterBaseAccess(flt).OnLumi(currentEvent, settings, m_globalMetadata);
					}
					const bool filterResult = FilterBaseAccess(flt).DoesEventPass(evtProvider.GetCurrentEvent(), productGlobal, settings, m_globalMetadata);
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

			for (PipelinesIterator pipeline = m_pipelines.begin(); pipeline != m_pipelines.end(); ++pipeline)
			{
				if (pipeline->GetSettings().GetLevel() == 1)
				{
					productGlobal.PreviousPipelinesResult = pipelineFilterRes;
					bool result = pipeline->RunEvent(evtProvider.GetCurrentEvent(), productGlobal, globalFilterResult);
					pipelineFilterRes.SetFilterDecision(pipeline->GetSettings().GetName(), result);
				}
			}
		}

		for (ProgressReportIterator report = m_progressReport.begin(); report != m_progressReport.end(); ++report)
		{
			report->finish();
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator pipeline = m_pipelines.begin(); pipeline != m_pipelines.end(); ++pipeline)
		{
			if (pipeline->GetSettings().GetLevel() == 1)
			{
				pipeline->FinishPipeline();
			}
		}

		osSignalReset();

		// run the pipelines greater level one
		// even if the previous event loop was interrupted
		bool noPipelineRun = false;
		size_t curLevel = 2;
		while (! noPipelineRun)
		{
			noPipelineRun = true;
			for (PipelinesIterator pipeline = m_pipelines.begin(); pipeline != m_pipelines.end(); ++pipeline)
			{
				if (osHasSIGINT())
				{
					LOG(INFO)<< "Terminating processing due to received SIGTERM";
					break;
				}

				if (pipeline->GetSettings().GetLevel() == curLevel)
				{
					noPipelineRun = false;

					pipeline->Run();
					pipeline->FinishPipeline();
				}
			}
			++curLevel;
		}
	}

	void AddProgressReport(ProgressReportBase* p)
	{
		m_progressReport.push_back(p);
	}

	void ClearProgressReports()
	{
		m_progressReport.clear();
	}

	Pipelines& GetPipelines()
	{
		return m_pipelines;
	}

	ProcessNodes& GetNodes()
	{
		return m_globalNodes;
	}

	metadata_type& GetGlobalMetadata()
	{
		return m_globalMetadata;
	}

private:

	Pipelines m_pipelines;
	ProcessNodes m_globalNodes;
	ProgressReportList m_progressReport;
	bool m_registerSignalHandler;
	metadata_type m_globalMetadata;
};
