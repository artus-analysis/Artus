
#pragma once

#include <string>
#include <boost/noncopyable.hpp>


#include "Artus/Utility/interface/ArtusLogging.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/ProcessNodeBase.h"

/**
   \brief Base class for Producers, that extend the product.
   
   This is a base class for Producers, that extend the product after performing
   some calculations. The function ProduceGlobal is called once per event before
   the pipelines are split. The function ProduceLocal is called once per event
   within a given pipeline. Only one of these functions are called.
*/

template<class TTypes>
class ProducerBase: public ProcessNodeBase {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual ~ProducerBase() {

	}

	virtual void InitGlobal(global_setting_type const& globalSettings) {
		LOG(DEBUG) << "Initialize global Producer \"" << this->GetProducerId() << "\".";
	}

	virtual void InitLocal(setting_type const& pipelineSettings) {
		LOG(DEBUG) << "Initialize local Producer \"" << this->GetProducerId() << "\" for pipeline \"" << settings.GetName() << "\".";
	}

	// called once per event before the pipelines are split
	virtual void ProduceGlobal(event_type const& event, product_type& product,
	                           global_setting_type const& globalSettings) const {
		LOG(FATAL) << "ProduceGlobal function for producer \"" << this->GetProducerId() << "\" is not implemented!";
	}

	// called once per event within a given pipeline
	virtual void ProduceLocal(event_type const& event, product_type & product, 
	                          setting_type const& pipelineSettings) const {
		LOG(FATAL) << "ProduceLocal function for producer \"" << this->GetProducerId() << "\" is not implemented!";
	}

	/// Must return a unique id of the producer.
	virtual std::string GetProducerId() const = 0;

	virtual ProcessNodeType GetProcessNodeType () const
		ARTUS_CPP11_FINAL
		ARTUS_CPP11_OVERRIDE
	{
		return ProcessNodeType::Producer;
	}
};

