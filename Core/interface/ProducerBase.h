
#pragma once

#include <boost/noncopyable.hpp>

#include "Artus/Core/interface/GlobalInclude.h"
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
	}

	virtual void InitLocal(setting_type const& pipelineSettings) {
	}

	/// if false is returned, the event is dropped as it does not meet the minimum requirements for the producer
	// called once per event before the pipelines are split
	virtual bool ProduceGlobal(event_type const& event, product_type& product,
	                           global_setting_type const& globalSettings) const {
                std::cout << "ProduceGlobal for producer " << this->GetProducerId() << " is not implemented" << std::endl;
                exit(1);
	}

	// called once per event within a given pipeline
	virtual void ProduceLocal(event_type const& event, product_type & product, 
	                          setting_type const& pipelineSettings) const {
		std::cout << "ProduceLocal for producer " << this->GetProducerId() << " is not implemented" << std::endl;
		exit(1); 
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

