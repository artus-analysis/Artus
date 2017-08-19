
#include "Artus/Core/interface/ProducerBase.h"


ProducerBaseUntemplated::~ProducerBaseUntemplated()
{
}

ProducerBaseAccess::ProducerBaseAccess(ProducerBaseUntemplated& producer) :
	m_producer(producer)
{
}

void ProducerBaseAccess::Init(SettingsBase const& settings)
{
	m_producer.baseInit(settings);
}

void ProducerBaseAccess::Produce(EventBase const& event, ProductBase& product, SettingsBase const& settings)
{
	m_producer.baseProduce(event, product, settings);
}
void ProducerBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings)
{
	m_producer.baseOnRun(event, settings);
}
void ProducerBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings)
{
	m_producer.baseOnLumi(event, settings);
}

