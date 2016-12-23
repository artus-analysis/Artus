
#include "Artus/Core/interface/ProducerBase.h"


ProducerBaseUntemplated::~ProducerBaseUntemplated()
{
}

std::string ProducerBaseUntemplated::GetProducerId() const
{
	return GetId();
}

ProducerBaseAccess::ProducerBaseAccess(ProducerBaseUntemplated& cb) :
		m_cb(cb)
{
}

void ProducerBaseAccess::Init(SettingsBase const& settings)
{
	m_cb.baseInit(settings);
}

void ProducerBaseAccess::Produce(EventBase const& event, ProductBase& product, SettingsBase const& settings)
{
	m_cb.baseProduce(event, product, settings);
}
void ProducerBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings)
{
	m_cb.baseOnRun(event, settings);
}
void ProducerBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings)
{
	m_cb.baseOnLumi(event, settings);
}

