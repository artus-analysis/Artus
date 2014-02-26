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
#include "LocalProducerBase.h"

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

	virtual void InitPipeline(pipeline_type * pLine,
			typename TTypes::setting_type const& pset) const = 0;

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
	typedef boost::ptr_vector<FilterBase<TTypes> > FilterVector;
	typedef typename FilterVector::iterator FilterVectorIterator;

	typedef LocalProducerBase<TTypes> ProducerForThisPipeline;

	typedef boost::ptr_vector<ProducerForThisPipeline> ProducerVector;
	typedef typename ProducerVector::iterator ProducerVectorIterator;

	/// Virtual constructor.
	virtual ~Pipeline() {
	}

	/// Initialize the pipeline using a custom PipelineInitilizer. This PipelineInitilizerBase 
	/// can create specific Filters and Consumers
	virtual void InitPipeline(setting_type pset,
			PipelineInitilizerBase<TTypes> const& initializer) {
		m_pipelineSettings = pset;
		initializer.InitPipeline(this, pset);

		// init Filters
		for (auto & it : m_filter) {
			it.Init(this);
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

		for (FilterVectorIterator itfilter = m_filter.begin();
				itfilter != m_filter.end(); itfilter++) {
			s << std::endl << itfilter->GetFilterId();
		}

		return s.str();
	}

	/// Called once all events have been passed to the pipeline.
	virtual void FinishPipeline() {
		for (auto & it : m_consumer) {
			it.Finish();
		}

		for (auto & it : m_filter) {
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
			product_type const& globalProduct) {

	        // make a local copy of the global product and allow this one to be modified by local 
	        // Producers.
		product_type localProduct ( globalProduct );

		// run local Producers
		for (ProducerVectorIterator it = m_producer.begin();
				it != m_producer.end(); it++) {
			it->ProduceLocal(evt, localProduct,
					m_pipelineSettings);
		}

		// run Filters
		FilterResult fres;
		for (FilterVectorIterator itfilter = m_filter.begin();
				itfilter != m_filter.end(); itfilter++) {
			fres.SetFilterDecisions(itfilter->GetFilterId(),
					itfilter->DoesEventPass(evt, localProduct,
							m_pipelineSettings));
		}

		// run Consumers
		for (ConsumerVectorIterator itcons = m_consumer.begin();
				itcons != m_consumer.end(); itcons++) {
			if (fres.HasPassed()) {
				itcons->ProcessFilteredEvent(evt, localProduct);
			}

			itcons->ProcessEvent(evt, localProduct, fres);
		}
	}

	/// Find and return a Filter by it's id in this pipeline.
	virtual FilterBase<TTypes>* FindFilter(std::string sFilterId) {
		for (FilterVectorIterator it = m_filter.begin(); it != m_filter.end();
				it++) {
			if (it->GetFilterId() == sFilterId)
				return &(*it);
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

		m_filter.push_back(pFilter);
	}

	/// Add a new Consumer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddConsumer(ConsumerForThisPipeline * pConsumer) {
		//std::cout << "=== AddConsumer" << std::endl;
		m_consumer.push_back(pConsumer);
		//std::cout << "size = " << m_consumer.size() << std::endl;
	}

	/// Add a new Producer to this Pipeline. The object will be freed in Pipelines destructor.
	virtual void AddProducer(ProducerForThisPipeline * pProd) {
		m_producer.push_back(pProd);
	}

	/// Return a list of filters is this pipeline.
	const FilterVector& GetFilters() {
		return m_filter;
	}

private:
	ConsumerVector m_consumer;
	FilterVector m_filter;
	ProducerVector m_producer;
	setting_type m_pipelineSettings;
};

