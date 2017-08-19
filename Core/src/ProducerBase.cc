
#include "Artus/Core/interface/ProducerBase.h"


ProducerBaseUntemplated::~ProducerBaseUntemplated()
{
}

ProducerBaseAccess::ProducerBaseAccess(ProducerBaseUntemplated& producer) :
	m_producer(producer)
{
}

void ProducerBaseAccess::Init(SettingsBase const& settings, MetadataBase& metadata)
{
	m_producer.baseInit(settings, metadata);
}

void ProducerBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_producer.baseOnRun(event, settings, metadata);
}

void ProducerBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_producer.baseOnLumi(event, settings, metadata);
}

void ProducerBaseAccess::Produce(EventBase const& event, ProductBase& product, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_producer.baseProduce(event, product, settings, metadata);
}

