
#pragma once

#include <string>
#include <boost/noncopyable.hpp>


#include "Artus/Utility/interface/ArtusLogging.h"

#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/MetadataBase.h"
#include "Artus/Core/interface/ProcessNodeBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"


/**
   \brief Base class for Producers, that extend the product.
   
   This is a base class for Producers, that extend the product after performing
   some calculations. The function ProduceGlobal is called once per event before
   the pipelines are split. The function ProduceLocal is called once per event
   within a given pipeline. Only one of these functions are called.
*/

class ProducerBaseAccess;

class ProducerBaseUntemplated : public ProcessNodeBase
{
public:

	// this will allow the pipeline class to call the protected
	// base implemenations of ProcessEvent / Init and friends
	friend ProducerBaseAccess;

	virtual ~ProducerBaseUntemplated();

	/// Must return a unique id of the producer.
	virtual std::string GetProducerId() const = 0;

protected:
	// will be implemented by the ConsumerBase class
	virtual void baseInit(SettingsBase const& settings, MetadataBase& metadata) = 0;
	
	virtual void baseOnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) = 0;
	virtual void baseOnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) = 0;

	virtual void baseProduce(EventBase const& event, ProductBase& product, SettingsBase const& settings, MetadataBase const& metadata) const = 0;
};


class ProducerBaseAccess
{
public:
	explicit ProducerBaseAccess(ProducerBaseUntemplated& producer);

	void Init(SettingsBase const& settings, MetadataBase& metadata);
	
	void OnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata);
	void OnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata);
	
	void Produce(EventBase const& event, ProductBase& product, SettingsBase const& settings, MetadataBase const& metadata);

private:
	ProducerBaseUntemplated& m_producer;
};

template<class TTypes>
class ProducerBase: public ProducerBaseUntemplated
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	virtual ~ProducerBase()
	{
	}

	virtual void Init(setting_type const& settings, metadata_type& metadata)
	{
		LOG(DEBUG) << "Initialize producer \"" << this->GetProducerId() << "\".";
	}
	
	virtual void OnLumi(event_type const& event, setting_type const& settings, metadata_type const& metadata)
	{
	}
	
	virtual void OnRun(event_type const& event, setting_type const& settings, metadata_type const& metadata)
	{
	}

	virtual void Produce(event_type const& event, product_type& product, setting_type const& settings, metadata_type const& metadata) const = 0;

	ProcessNodeType GetProcessNodeType() const final
	{
		return ProcessNodeType::Producer;
	}

protected:

	void baseInit(SettingsBase const& settings, MetadataBase& metadata) override
	{
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type& specMetadata = static_cast<metadata_type&>(metadata);

		this->Init(specSettings, specMetadata);
	}

	void baseOnRun(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata) override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		OnRun(specEvent, specSettings, specMetadata);
	}
	
	void baseOnLumi(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata) override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		OnLumi(specEvent, specSettings, specMetadata);
	}

	void baseProduce(EventBase const& evt, ProductBase& prod, SettingsBase const& settings, MetadataBase const& metadata) const override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		product_type& specProduct = static_cast<product_type&>(prod);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		Produce(specEvent, specProduct, specSettings, specMetadata);
	}
};

