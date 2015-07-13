/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <vector>
#include <sstream>
#include <time.h>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Artus/Utility/interface/Collections.h"

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
class PipelineInitilizerBase {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef Pipeline<TTypes> pipeline_type;

	virtual void InitPipeline(pipeline_type * pLine, setting_type const& pset) const {};

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
class Pipeline: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef ConsumerBaseUntemplated ConsumerForThisPipeline;
	typedef boost::ptr_vector<ConsumerBaseUntemplated> ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBaseUntemplated FilterForThisPipeline;
	typedef ProducerBaseUntemplated ProducerForThisPipeline;

	typedef boost::ptr_vector< ProcessNodeBase > ProcessNodeVector;
	typedef typename ProcessNodeVector::iterator ProcessNodeIterator;

	/// Virtual constructor.
	virtual ~Pipeline() {
	}

	/// Initialize the pipeline using a custom PipelineInitilizer. This PipelineInitilizerBase 
	/// can create specific Filters and Consumers
	virtual void InitPipeline(setting_type pset,
			PipelineInitilizerBase<TTypes> const& initializer) {
		
		LOG(DEBUG) << "";
		LOG(DEBUG) << "Initialize pipeline \"" << pset.GetName() << "\".";
		
		m_pipelineSettings = pset;
		initializer.InitPipeline(this, pset);

		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {
			if ( it->GetProcessNodeType () == ProcessNodeType::Producer ){
				ProducerBaseAccess(	static_cast< ProducerForThisPipeline &> ( *it )	)
						. Init ( pset );
			}
			else if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				FilterBaseAccess( static_cast< FilterForThisPipeline &> ( *it ) )
						. Init ( pset );
			}
			else {
				LOG(FATAL) << "ProcessNodeType not supported by the pipeline!";
			}
		}

		// init Consumers
		for (auto & it : m_consumer) {
			ConsumerBaseAccess(it).Init( pset );
		}

		// store the filter names for later use in RunEvent
		m_filterNames = pset.GetFilters();
	}

	/// Useful debug output of the Pipeline Content.
	virtual std::string GetContent() {
		std::stringstream s;
		s << "== Pipeline Settings: " << std::endl;
		s << m_pipelineSettings.ToString() << std::endl;
		s << "== Pipeline Filter: ";

		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {
			if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				s << std::endl << static_cast<FilterForThisPipeline &> ( *it ) . GetFilterId();
			}
		}

		return s.str();
	}

	/// Called once all events have been passed to the pipeline.
	virtual void FinishPipeline() {
		for (auto & it : m_consumer) {
			ConsumerBaseAccess( it ).Finish( GetSettings() );
		}
	}

	/// Run the pipeline without specific event input. This is most useful for Pipelines which 
	/// process output from Pipelines already run.
	virtual void Run() {
		for (auto & it : m_consumer) {
			ConsumerBaseAccess( it ).Process( GetSettings() );
		}
	}

	/// Run the pipeline with one specific event as input. GlobalProduct are products which are 
	/// common for all pipelines and have therefore been created only once.
	virtual bool RunEvent(event_type const& evt,
			product_type const& globalProduct,
			FilterResult const& globalFilterResult) {

		// make a local copy of the global product/filter result
		// and allow this one to be modified by local producers/filters.
		product_type localProduct ( globalProduct );
		FilterResult localFilterResult ( globalFilterResult );
		localFilterResult.AddFilterNames( m_filterNames );

		// run Filters & Producers
		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {

			// variables for runtime measurement
			timeval tStart, tEnd;
			int runTime;

			// stop processing as soon as one filter fails
			// but the consumers will still be processed
			// this will also stop processing, if a global filter
			// already failed
			if (! localFilterResult.HasPassed())
				break;
			
			if ( it->GetProcessNodeType () == ProcessNodeType::Producer ){
				ProducerForThisPipeline& prod = static_cast<ProducerForThisPipeline&>(*it);
				//LOG(DEBUG) << prod.GetProducerId() << "::Produce (pipeline: " << m_pipelineSettings.GetName() << ")";
				gettimeofday(&tStart, 0);
				ProducerBaseAccess(prod).Produce(evt, localProduct, m_pipelineSettings);
				gettimeofday(&tEnd, 0);
				runTime = ( tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec );
				localProduct.processorRunTime[prod.GetProducerId()] = runTime;
			}
			else if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				FilterForThisPipeline & flt = static_cast<FilterForThisPipeline&>(*it);
				//LOG(DEBUG) << flt.GetFilterId() << "::DoesEventPass (pipeline: " << m_pipelineSettings.GetName() << ")";
				gettimeofday(&tStart, 0);
				const bool filterResult = FilterBaseAccess(flt).DoesEventPass(evt, localProduct, m_pipelineSettings);
				localFilterResult.SetFilterDecision(flt.GetFilterId(), filterResult);
				gettimeofday(&tEnd, 0);
				runTime = ( tEnd.tv_sec * 1000000 + tEnd.tv_usec - tStart.tv_sec * 1000000 - tStart.tv_usec );
				localProduct.processorRunTime[flt.GetFilterId()] = runTime;
			}
			else {
				LOG(FATAL) << "ProcessNodeType not supported by the pipeline!";
			}
		}

		// run Consumers
		for (ConsumerVectorIterator itcons = m_consumer.begin(); itcons != m_consumer.end(); itcons++) {
			//LOG(DEBUG) << itcons->GetConsumerId() << "::ProcessFilteredEvent/ProcessEvent (pipeline: " << m_pipelineSettings.GetName() << ")";
			if (localFilterResult.HasPassed()) {
				ConsumerBaseAccess(*itcons).ProcessFilteredEvent(evt, localProduct, GetSettings());
			}

			ConsumerBaseAccess(*itcons).ProcessEvent(evt, localProduct, GetSettings(), localFilterResult);
		}

		return localFilterResult.HasPassed();
	}

	/// Find and return a Filter by it's id in this pipeline.
	virtual FilterBaseUntemplated* FindFilter(std::string sFilterId) {
		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {
			if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				FilterForThisPipeline * filter = &( static_cast<FilterForThisPipeline&> ( *it ) );
				if ( filter->GetFilterId() == sFilterId )
					return filter;
			}
		}

		return NULL;
	}

	/// Return a reference to the settings used within this pipeline.
	virtual setting_type const& GetSettings() const {
		return m_pipelineSettings;
	}

	/// Add a new Filter to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddFilter(FilterForThisPipeline * pFilter) {
		if (FindFilter(pFilter->GetFilterId()) != NULL)
			throw std::exception();

		m_nodes.push_back(pFilter);
	}

	/// Add a new Consumer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddConsumer(ConsumerForThisPipeline * pConsumer) {
		m_consumer.push_back(pConsumer);
	}

	/// Add a new Producer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddProducer(ProducerForThisPipeline * pProd) {
		m_nodes.push_back ( pProd );
	}

	ProcessNodeVector & GetNodes () {
		return m_nodes;
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
	stringvector m_filterNames;
};

