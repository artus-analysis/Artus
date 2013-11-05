#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

// forward define to be able to use the event pipeline here
template<class TTypes>
class EventPipeline;

template<class TTypes>
class FilterBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::global_meta_type global_meta_type;
	typedef typename TTypes::local_meta_type local_meta_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~FilterBase() {
	}

	// todo: do we event need this init this ?
	virtual void Init(EventPipeline<TTypes> * pset) {
		//m_pipeline = pset;
	}

	virtual void Finish() {
	}

	virtual std::string GetFilterId() = 0;

	virtual bool DoesEventPass(event_type const& event,
			global_meta_type const& metaData, setting_type const& settings) = 0;

	virtual std::string ToString(bool bVerbose = false) {
		return GetFilterId();
	}

};
