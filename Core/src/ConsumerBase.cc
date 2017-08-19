
#include "Artus/Core/interface/ConsumerBase.h"


ConsumerBaseUntemplated::~ConsumerBaseUntemplated()
{
}


ConsumerBaseAccess::ConsumerBaseAccess(ConsumerBaseUntemplated& consumer) :
	m_consumer(consumer)
{
}

void ConsumerBaseAccess::Init(SettingsBase const& settings, MetadataBase& metadata)
{
	m_consumer.baseInit(settings, metadata);
}

void ConsumerBaseAccess::OnLumi(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_consumer.baseOnLumi(evt, settings, metadata);
}

void ConsumerBaseAccess::OnRun(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_consumer.baseOnRun(evt, settings, metadata);
}

void ConsumerBaseAccess::Process(SettingsBase const& settings, MetadataBase const& metadata)
{
	m_consumer.baseProcess(settings, metadata);
}

void ConsumerBaseAccess::ProcessEvent(EventBase const& evt, ProductBase const& prod, SettingsBase const& settings, MetadataBase const& metadata, FilterResult fres)
{
	m_consumer.baseProcessEvent(evt, prod, settings, metadata, fres);
}

void ConsumerBaseAccess::ProcessFilteredEvent(EventBase const& evt, ProductBase const& prod, SettingsBase const& settings, MetadataBase const& metadata)
{
	m_consumer.baseProcessFilteredEvent(evt, prod, settings, metadata);
}

void ConsumerBaseAccess::Finish(SettingsBase const& settings, MetadataBase const& metadata)
{
	m_consumer.baseFinish(settings, metadata);
}
