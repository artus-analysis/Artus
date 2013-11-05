#pragma once

#include <boost/noncopyable.hpp>

// Basis for producers which are run before the pipelines and produce
// results useful to all
template<class TTypes>
class GlobalMetaDataProducerBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::global_meta_type global_meta_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~GlobalMetaDataProducerBase() {

	}

	virtual void Init(setting_type const& globalSettings) {
	}

	// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	virtual bool PopulateGlobalMetaData(event_type const& data,
			global_meta_type & metaData,
			setting_type const& globalSettings) const = 0;
};

