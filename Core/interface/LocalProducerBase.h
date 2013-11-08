/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

/*
 * This producer creates products for a pipeline and event before the filter or the consumer are run.
 * Producers have to be stateless since they may used by multiple threads.
 */

// Basis for producers attached to a pipeline and producing local results for this pipeline
template<class TTypes>
class LocalProducerBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::global_product_type global_product_type;
	typedef typename TTypes::local_product_type local_product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~LocalProducerBase() {

	}

	virtual void ProduceLocal(event_type const& data,
			global_product_type const& globalProduct, local_product_type& localProduct,
			setting_type const& m_pipelineSettings) const = 0;

};

