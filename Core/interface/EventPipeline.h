#pragma once

#include <vector>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "PipelineSettings.h"
#include "FilterBase.h"
#include "EventConsumerBase.h"
#include "MetaDataProducerBase.h"

template<class TTypes>
class EventPipeline;

/*
 Base class for your custom PipelineInitializer. Your custom code
 can add Filters and Consumers to newly created pipelines.
 */

template<class TTypes>
class PipelineInitilizerBase {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::local_meta_type local_meta_type;
	typedef typename TTypes::global_meta_type global_meta_type;
	typedef typename TTypes::setting_type setting_type;

	typedef EventPipeline<TTypes> pipeline_type;

	virtual void InitPipeline(pipeline_type * pLine,
			typename TTypes::setting_type const& pset) const = 0;

};

/*

 \brief Base implementation of the EventPipeline paradigm

 The EventPipline contains settings, filter and MetaDataProducer and Consumer which, when combined,
 produce the desired output of a pipeline as soon as Events are send to the pipeline. An incoming event
 must not be changed by the pipeline but the pipeline can create additional data for an event using
 MetaDataProducers.
 Most of the time, the EventPipeline will not be used stand-alone but by an EventPipelineRunner class.

 The intention of the
 different components is outlined in the following:


 - Settings
 Contain all specifics for the behaviour of this pipeline. The Settings object of type TSettings must be used
 to steer the behaviour of the MetaDataProducers, Filters and Consumers

 - MetaDataProducers
 Create additional, pipeline-specific, data for an event and stores this information in a TMetaData object

 - Filter
 Filter decide whether an input event is suitable to be processed by this pipeline. An event might not be in the desired
 PtRange and is therefore not useful for this pipeline. The FilterResult is stored and Consumers can access the outcome of the
 Filter process.

 - Consumer
 The Consumer can access the input event, the created metadata, the settings and the filter result and produce the output they
 desire, like Histograms -> PLOTS PLOTS PLOTS

 Execution order is easy:
 MetaDataProducers -> Filters -> Consumers

 */

template<class TTypes>
class EventPipeline: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::local_meta_type local_meta_type;
	typedef typename TTypes::global_meta_type global_meta_type;
	typedef typename TTypes::setting_type setting_type;

	typedef EventConsumerBase<TTypes> ConsumerForThisPipeline;
	typedef boost::ptr_vector<EventConsumerBase<TTypes> > ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBase<TTypes> FilterForThisPipeline;
	typedef boost::ptr_vector<FilterBase<TTypes> > FilterVector;
	typedef typename FilterVector::iterator FilterVectorIterator;

	typedef LocalMetaDataProducerBase<TTypes> MetaDataProducerForThisPipeline;

	typedef boost::ptr_vector<MetaDataProducerForThisPipeline> MetaDataProducerVector;
	typedef typename MetaDataProducerVector::iterator MetaDataVectorIterator;

	/*
	 * Virtual constructor
	 */
	virtual ~EventPipeline() {
	}

	/*
	 Initialize the pipeline using a custom PipelineInitilizer. This PipelineInitilizerBase can
	 create specific Filters and Consumers
	 */
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

	/*
	 Useful debug output of the Pipeline Content
	 */
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

	/*
	 Called once all events have been passed to the pipeline
	 */
	virtual void FinishPipeline() {
		for (auto & it : m_consumer) {
			it.Finish();
		}

		for (auto & it : m_filter) {
			it.Finish();
		}
	}

	/*
	 * Run the pipeline without specific event input. This is most useful for
	 * Pipelines which process output from Pipelines already run.
	 */
	virtual void Run() {
		for (auto & it : m_consumer) {
			it.Process();
		}
	}

	/*
	 * Run the pipeline with one specific event as input
	 * The globalMetaData is meta data which is equal for all pipelines and has therefore
	 * been created only once.
	 */
	virtual void RunEvent(event_type const& evt,
			global_meta_type const& globalMetaData) {
		// create the pipeline local data and set the pointer to the localMetaData
		//global_meta_type & nonconst_metaData = const_cast<global_meta_type&>(globalMetaData);
		local_meta_type localMetaData;
		//nonconst_metaData.SetLocalMetaData(&localMetaData);

		// run MetaDataProducers
		// Pipeline private MetaDataProducers not supported at the moment
		for (MetaDataVectorIterator it = m_producer.begin();
				it != m_producer.end(); it++) {
			it->PopulateLocal(evt, globalMetaData, localMetaData,
					m_pipelineSettings);
		}

		// run Filters
		FilterResult fres;
		for (FilterVectorIterator itfilter = m_filter.begin();
				itfilter != m_filter.end(); itfilter++) {
			fres.SetFilterDecisions(itfilter->GetFilterId(),
					itfilter->DoesEventPass(evt, globalMetaData,
							m_pipelineSettings));
		}

		// run Consumer
		for (ConsumerVectorIterator itcons = m_consumer.begin();
				itcons != m_consumer.end(); itcons++) {
			if (fres.HasPassed())
				itcons->ProcessFilteredEvent(evt, globalMetaData,
						localMetaData);

			itcons->ProcessEvent(evt, globalMetaData, localMetaData, fres);
		}
	}

	/*
	 * Find and return a Filter by it's id in this pipeline
	 */
	virtual FilterBase<TTypes>* FindFilter(std::string sFilterId) {
		for (FilterVectorIterator it = m_filter.begin(); it != m_filter.end();
				it++) {
			if (it->GetFilterId() == sFilterId)
				return &(*it);
		}
		return NULL;
	}

	/*
	 * Return a reference to the settings used within this pipeline
	 */
	virtual setting_type const& GetSettings() const {
		return m_pipelineSettings;
	}

	/*
	 * Add a new Filter to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddFilter(FilterForThisPipeline * pFilter) {
		if (FindFilter(pFilter->GetFilterId()) != NULL)
			throw std::exception();

		m_filter.push_back(pFilter);
	}

	/*
	 * Add a new Consumer to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddConsumer(ConsumerForThisPipeline * pConsumer) {
		//std::cout << "=== AddConsumer" << std::endl;
		m_consumer.push_back(pConsumer);
		//std::cout << "size = " << m_consumer.size() << std::endl;
	}

	/*
	 * Add a new MetaDataProducer to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddMetaDataProducer(MetaDataProducerForThisPipeline * pProd) {
		m_producer.push_back(pProd);
	}

	/*
	 * Return a list of filters is this pipeline
	 */
	const FilterVector& GetFilters() {
		return m_filter;
	}

private:
	ConsumerVector m_consumer;
	FilterVector m_filter;
	MetaDataProducerVector m_producer;
	setting_type m_pipelineSettings;
};

