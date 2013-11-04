#pragma once

#include <boost/noncopyable.hpp>

/*
 * This producer creates meta-data for a pipeline and event before the filter or the consumer are run
 * Meta data producer have to be stateless since they may used by multiple threads
 */

// Basis for producers attached to a pipeline and producing local results for this pipeline
template<class TTypes>
class LocalMetaDataProducerBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::meta_type meta_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~LocalMetaDataProducerBase() {

	}

	virtual void PopulateLocal(event_type const& data,
			meta_type const& metaData,
			typename meta_type::LocalMetaDataType & localMetaData,
			setting_type const& m_pipelineSettings) const = 0;

};

// Basis for producers which are run before the pipelines and produce 
// results useful to all
template<class TTypes>
class GlobalMetaDataProducerBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::meta_type meta_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~GlobalMetaDataProducerBase() {

	}

	virtual void Init(setting_type const& globalSettings) {
	}

	// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool PopulateGlobalMetaData(event_type const& data,
			meta_type & metaData, setting_type const& globalSettings) const = 0;
};

