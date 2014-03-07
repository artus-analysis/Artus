/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <vector>
#include <sstream>

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
   
   - LocalProducers
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

	typedef ConsumerBase<TTypes> ConsumerForThisPipeline;
	typedef boost::ptr_vector<ConsumerBase<TTypes> > ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBase<TTypes> FilterForThisPipeline;
	typedef ProducerBase<TTypes> ProducerForThisPipeline;

	typedef boost::ptr_vector< ProcessNodeBase > ProcessNodeVector;
	typedef typename ProcessNodeVector::iterator ProcessNodeIterator;

	/// Virtual constructor.
	virtual ~Pipeline() {
	}

	/// Initialize the pipeline using a custom PipelineInitilizer. This PipelineInitilizerBase 
	/// can create specific Filters and Consumers
	virtual void InitPipeline(setting_type pset,
			PipelineInitilizerBase<TTypes> const& initializer) {
		m_pipelineSettings = pset;
		initializer.InitPipeline(this, pset);

		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {
			if ( it->GetProcessNodeType () == ProcessNodeType::Producer ){
				static_cast< ProducerForThisPipeline &> ( *it ) . InitLocal( pset );
			}
			else if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				static_cast< FilterForThisPipeline &> ( *it ) . InitLocal ( pset );
			}
			else {
				LOG_FATAL( "ProcessNodeType not supported by the pipeline" );
			}
		}

		// init Consumers
		for (auto & it : m_consumer) {
			it.Init(this);
		}
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
			it.Finish();
		}
	}

	/// Run the pipeline without specific event input. This is most useful for Pipelines which 
	/// process output from Pipelines already run.
	virtual void Run() {
		for (auto & it : m_consumer) {
			it.Process();
		}
	}

	/// Run the pipeline with one specific event as input. GlobalProduct are products which are 
	/// common for all pipelines and have therefore been created only once.
	virtual void RunEvent(event_type const& evt,
			product_type const& globalProduct,
			FilterResult const& globalFilterResult) {

		// make a local copy of the global product/filter result
		// and allow this one to be modified by local producers/filters.
		product_type localProduct ( globalProduct );
		FilterResult localFilterResult ( globalFilterResult );

		// run Filters & Producers
		for( ProcessNodeIterator it = m_nodes.begin();
				it != m_nodes.end(); it ++ ) {

			// stop processing as soon as one filter fails
			// but the consumers will still be processed
			if (! localFilterResult.HasPassed())
				break;
			
			if ( it->GetProcessNodeType () == ProcessNodeType::Producer ){
				static_cast< ProducerForThisPipeline &> ( *it ) . ProduceLocal(evt, localProduct,
						m_pipelineSettings);
			}
			else if ( it->GetProcessNodeType () == ProcessNodeType::Filter ) {
				FilterForThisPipeline & flt = static_cast<FilterForThisPipeline &> ( *it );
				const bool filterResult = flt . DoesEventPassLocal(evt, localProduct,
				                                                   m_pipelineSettings);
				localFilterResult.SetFilterDecision( flt.GetFilterId(), filterResult );
			}
			else {
				LOG_FATAL( "ProcessNodeType not supported by the pipeline" );
			}
		}

		// run Consumers
		for (ConsumerVectorIterator itcons = m_consumer.begin();
				itcons != m_consumer.end(); itcons++) {
			if (localFilterResult.HasPassed()) {
				itcons->ProcessFilteredEvent(evt, localProduct);
			}

			itcons->ProcessEvent(evt, localProduct, localFilterResult);
		}
	}

	/// Find and return a Filter by it's id in this pipeline.
	virtual FilterBase<TTypes>* FindFilter(std::string sFilterId) {
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
};

