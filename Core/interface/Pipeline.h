
#pragma once

#include <vector>
#include <sstream>
#include <time.h>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "PipelineSettings.h"
#include "FilterBase.h"
#include "ConsumerBase.h"
#include "ProducerBase.h"

template<class TTypes>
class Pipeline;

/**
   \brief Base class for your custom PipelineInitializer.

   Your custom code can add Filters and Consumers to newly created pipelines.
 */

template<class TTypes>
class PipelineInitilizerBase
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	typedef Pipeline<TTypes> pipeline_type;

	virtual void InitPipeline(pipeline_type* pLine, setting_type const& pset, metadata_type& metadata) const
	{
	};

};

/**
   \brief Base implementation of the Pipeline paradigm.

   The Pipline contains settings, producer, filter and consumer which, when combined, produce the
   desired output of a pipeline as soon as Events are send to the pipeline. An incoming event must
   not be changed by the pipeline but the pipeline can create additional data for an event using
   Producers. Most of the time, the Pipeline will not be used stand-alone but by an PipelineRunner
   class.

   The intention of the different components is outlined in the following:


   - Settings
   Contain all specifics for the behaviour of this pipeline. The Settings object of type TSettings
   must be used to steer the behaviour of the Producers, Filters and Consumers.

   - Producers
   Create additional, pipeline-specific, data for an event and stores this information in a TProduct
   object.

   - Filters
   Decide whether an input event is suitable to be processed by this pipeline. An event might not be
   in the desired PtRange and is therefore not useful for this pipeline. The FilterResult is stored
   and Consumers can access the outcome of the Filter process.

   - Consumers
   Can access the input event, the created products, the settings and the filter result and produce
   the output they desire, like Histograms -> PLOTS PLOTS PLOTS

   Execution order is: Producers -> Filters -> Consumers. Each pipeline can have several Producers,
   Filters or Consumers.

*/

template<class TTypes>
class Pipeline: public boost::noncopyable
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	typedef ConsumerBaseUntemplated ConsumerForThisPipeline;
	typedef boost::ptr_vector<ConsumerBaseUntemplated> ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBaseUntemplated FilterForThisPipeline;
	typedef ProducerBaseUntemplated ProducerForThisPipeline;

	typedef boost::ptr_vector< ProcessNodeBase > ProcessNodeVector;
	typedef typename ProcessNodeVector::iterator ProcessNodeIterator;

	/// Virtual constructor.
	virtual ~Pipeline()
	{
	}

	/// Initialize the pipeline using a custom PipelineInitilizer. This PipelineInitilizerBase
	/// can create specific Filters and Consumers
	virtual void InitPipeline(setting_type pset, metadata_type globalMetadata, PipelineInitilizerBase<TTypes> const& initializer)
	{
		LOG(DEBUG) << "";
		LOG(DEBUG) << "Initialize pipeline \"" << pset.GetName() << "\"...";

		m_pipelineSettings = pset;
		m_metadata = globalMetadata;

		initializer.InitPipeline(this, pset, m_metadata);

		for(ProcessNodeIterator processNode = m_nodes.begin(); processNode != m_nodes.end(); ++processNode)
		{
			if (processNode->GetProcessNodeType() == ProcessNodeType::Producer)
			{
				ProducerBaseAccess(static_cast<ProducerForThisPipeline&>(*processNode)).Init(pset, m_metadata);
			}
			else if (processNode->GetProcessNodeType() == ProcessNodeType::Filter)
			{
				FilterBaseAccess(static_cast<FilterForThisPipeline&>(*processNode)).Init(pset, m_metadata);
			}
			else
			{
				LOG(FATAL) << "ProcessNodeType not supported by the pipeline!";
			}
		}

		// init Consumers
		for (ConsumerForThisPipeline& consumer : m_consumer)
		{
			ConsumerBaseAccess(consumer).Init(pset, m_metadata);
		}

		// store the filter names for later use in RunEvent
		m_filterNames = pset.GetFilters();
		m_taggingFilters = pset.GetTaggingFilters();
	}

	/// Useful debug output of the Pipeline Content.
	virtual std::string GetContent()
	{
		std::stringstream s;
		s << "== Pipeline Settings: " << std::endl;
		s << m_pipelineSettings.ToString() << std::endl;
		s << "== Pipeline Filter: ";

		for (ProcessNodeIterator processNode = m_nodes.begin(); processNode != m_nodes.end(); ++processNode)
		{
			if (processNode->GetProcessNodeType () == ProcessNodeType::Filter)
			{
				s << std::endl << static_cast<FilterForThisPipeline&>(*processNode).GetFilterId();
			}
		}

		return s.str();
	}

	/// Called once all events have been passed to the pipeline.
	virtual void FinishPipeline()
	{
		for (ConsumerForThisPipeline& consumer : m_consumer)
		{
			ConsumerBaseAccess(consumer).Finish(GetSettings(), m_metadata);
		}
	}

	/// Run the pipeline without specific event input. This is most useful for Pipelines which
	/// process output from Pipelines already run.
	virtual void Run()
	{
		for (ConsumerForThisPipeline& consumer : m_consumer)
		{
			ConsumerBaseAccess(consumer).Process(GetSettings(), m_metadata);
		}
	}

	/// Run the pipeline with one specific event as input. GlobalProduct are products which are
	/// common for all pipelines and have therefore been created only once.
	virtual bool RunEvent(event_type const& evt, product_type const& globalProduct, FilterResult const& globalFilterResult)
	{
		// make a local copy of the global product/filter result
		// and allow this one to be modified by local producers/filters.
		product_type localProduct(globalProduct);
		FilterResult localFilterResult(globalFilterResult);
		localFilterResult.AddFilterNames(m_filterNames, m_taggingFilters);

		// run Filters & Producers
		for (ProcessNodeIterator processNode = m_nodes.begin(); processNode != m_nodes.end(); ++processNode)
		{
			// variables for runtime measurement
			timeval tStart, tEnd;
			int runTime;

			// stop processing as soon as one filter fails
			// but the consumers will still be processed
			// this will also stop processing, if a global filter
			// already failed
			if (! localFilterResult.HasPassed())
			{
				break;
			}

			if (processNode->GetProcessNodeType () == ProcessNodeType::Producer)
			{
				ProducerForThisPipeline& prod = static_cast<ProducerForThisPipeline&>(*processNode);
				gettimeofday(&tStart, nullptr);

				if (globalProduct.newRun)
				{
					ProducerBaseAccess(prod).OnRun(evt, m_pipelineSettings, m_metadata);
				}
				if (globalProduct.newLumisection)
				{
						ProducerBaseAccess(prod).OnLumi(evt, m_pipelineSettings, m_metadata);
				}
				ProducerBaseAccess(prod).Produce(evt, localProduct, m_pipelineSettings, m_metadata);

				gettimeofday(&tEnd, nullptr);
				runTime = static_cast<int>(tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec);  // a long int might be needed here but SafeMaps for long ints are not yet working
				localProduct.processorRunTime[prod.GetProducerId()] = runTime;
			}
			else if (processNode->GetProcessNodeType () == ProcessNodeType::Filter)
			{
				FilterForThisPipeline& flt = static_cast<FilterForThisPipeline&>(*processNode);
				gettimeofday(&tStart, nullptr);

				if(globalProduct.newRun)
				{
					FilterBaseAccess(flt).OnRun(evt, m_pipelineSettings, m_metadata);
				}
				if(globalProduct.newLumisection)
				{
					FilterBaseAccess(flt).OnLumi(evt, m_pipelineSettings, m_metadata);
				}
				const bool filterResult = FilterBaseAccess(flt).DoesEventPass(evt, localProduct, m_pipelineSettings, m_metadata);
				localFilterResult.SetFilterDecision(flt.GetFilterId(), filterResult);

				gettimeofday(&tEnd, nullptr);
				runTime = static_cast<int>(tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec);  // a long int might be needed here but SafeMaps for long ints are not yet working
				localProduct.processorRunTime[flt.GetFilterId()] = runTime;
			}
			else
			{
				LOG(FATAL) << "ProcessNodeType not supported by the pipeline!";
			}
		}
		localProduct.fres = localFilterResult;

		// run Consumers
		for (ConsumerVectorIterator consumer = m_consumer.begin(); consumer != m_consumer.end(); ++consumer)
		{
			if (globalProduct.newRun)
			{
				ConsumerBaseAccess(*consumer).OnRun(evt, GetSettings(), m_metadata);
			}
			if (globalProduct.newLumisection)
			{
				ConsumerBaseAccess(*consumer).OnLumi(evt, GetSettings(), m_metadata);
			}
			if (localFilterResult.HasPassed())
			{
				ConsumerBaseAccess(*consumer).ProcessFilteredEvent(evt, localProduct, GetSettings(), m_metadata);
			}

			ConsumerBaseAccess(*consumer).ProcessEvent(evt, localProduct, GetSettings(), m_metadata, localFilterResult);
		}

		return localFilterResult.HasPassed();
	}

	/// Find and return a Filter by it's id in this pipeline.
	virtual FilterBaseUntemplated* FindFilter(std::string sFilterId)
	{
		for (ProcessNodeIterator processNode = m_nodes.begin(); processNode != m_nodes.end(); ++processNode)
		{
			if (processNode->GetProcessNodeType () == ProcessNodeType::Filter)
			{
				FilterForThisPipeline* filter = &(static_cast<FilterForThisPipeline&>(*processNode));
				if (filter->GetFilterId() == sFilterId)
				{
					return filter;
				}
			}
		}

		return nullptr;
	}

	/// Return a reference to the settings used within this pipeline.
	virtual setting_type const& GetSettings() const
	{
		return m_pipelineSettings;
	}

	/// Add a new Filter to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddFilter(FilterForThisPipeline* pFilter)
	{
		if (FindFilter(pFilter->GetFilterId()) != nullptr)
		{
			throw std::exception();
		}

		m_nodes.push_back(pFilter);
	}

	/// Add a new Consumer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddConsumer(ConsumerForThisPipeline* pConsumer)
	{
		m_consumer.push_back(pConsumer);
	}

	/// Add a new Producer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddProducer(ProducerForThisPipeline* pProd)
	{
		m_nodes.push_back(pProd);
	}

	ProcessNodeVector& GetNodes()
	{
		return m_nodes;
	}

	metadata_type& GetMetadata()
	{
		return m_metadata;
	}

	/// Return a list of filters is this pipeline.
	/*
	 * disabled for now, if you need this again, contact Thomas
	const FilterVector& GetFilters() {
		return m_filter;
	}*/

private:
	ConsumerVector m_consumer;
	ProcessNodeVector m_nodes;
	setting_type m_pipelineSettings;
	std::vector<std::string> m_filterNames;
	std::vector<std::string> m_taggingFilters;
	metadata_type m_metadata;
};
