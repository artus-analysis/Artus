
#pragma once

#include <string>
#include <boost/noncopyable.hpp>


#include "Artus/Utility/interface/ArtusLogging.h"

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/EventBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"

#include "Artus/Core/interface/ProcessNodeBase.h"


/**
   \brief Base class for Producers, that extend the product.
   
   This is a base class for Producers, that extend the product after performing
   some calculations. The function ProduceGlobal is called once per event before
   the pipelines are split. The function ProduceLocal is called once per event
   within a given pipeline. Only one of these functions are called.
*/

class ProducerBaseAccess;

class ProducerBaseUntemplated : public ProcessNodeBase {
public:

	// this will allow the pipeline class to call the protected
	// base implemenations of ProcessEvent / Init and friends
	friend ProducerBaseAccess;

	virtual ~ProducerBaseUntemplated() {}

	/// Must return a unique id of the producer.
	virtual std::string GetProducerId() const = 0;

protected:
	// will be implemented by the ConsumerBase class
	virtual void baseInit ( SettingsBase const& settings ) = 0;

	virtual void baseProduce(EventBase const& event, ProductBase& product,
	                     SettingsBase const& settings) const = 0;
};


class ProducerBaseAccess {
public:
	ProducerBaseAccess ( ProducerBaseUntemplated & cb ) : m_cb(cb) {

	}

	void Init ( SettingsBase const& settings ){
		m_cb.baseInit( settings );
	}

	void Produce(EventBase const& event, ProductBase& product,
	                     SettingsBase const& settings){
		m_cb.baseProduce ( event, product, settings);
	}

private:
	ProducerBaseUntemplated & m_cb;
};

template<class TTypes>
class ProducerBase: public ProducerBaseUntemplated {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~ProducerBase() {

	}

	virtual void Init(setting_type const& globalSettings) {
		LOG(DEBUG) << "Initialize producer \"" << this->GetProducerId() << "\".";
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& globalSettings) const = 0;

	virtual ProcessNodeType GetProcessNodeType () const
		ARTUS_CPP11_FINAL
		ARTUS_CPP11_OVERRIDE
	{
		return ProcessNodeType::Producer;
	}

protected:

	virtual void baseProduce(EventBase const& evt,
			ProductBase & prod,
			SettingsBase const& setting ) const ARTUS_CPP11_OVERRIDE {
		auto const& specEvent = static_cast < event_type const&> ( evt );
		auto & specProd = static_cast < product_type &> ( prod );
		auto const& specSetting = static_cast < setting_type const&> ( setting );

		Produce( specEvent, specProd, specSetting );
	}

	virtual void baseInit ( SettingsBase const& settings ) ARTUS_CPP11_OVERRIDE {
		auto const& specSettings = static_cast < setting_type const&> ( settings );

		this->Init ( specSettings );
	}
};

