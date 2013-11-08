/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

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
	typedef typename TTypes::global_meta_type global_meta_type;
	typedef typename TTypes::local_meta_type local_meta_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~LocalMetaDataProducerBase() {

	}

	virtual void PopulateLocal(event_type const& data,
			global_meta_type const& metaData, local_meta_type & localMetaData,
			setting_type const& m_pipelineSettings) const = 0;

};

