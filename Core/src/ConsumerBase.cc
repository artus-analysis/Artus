
#include "Artus/Core/interface/ConsumerBase.h"


ConsumerBaseUntemplated::~ConsumerBaseUntemplated()
{
}


explicit ConsumerBaseAccess::ConsumerBaseAccess(ConsumerBaseUntemplated& consumer) :
	m_consumer(consumer)
{
}

void ConsumerBaseAccess::Process(SettingsBase const& settings)
{
	m_consumer.baseProcess(settings);
}

void ConsumerBaseAccess::ProcessEvent(EventBase const& evt, ProductBase const& prod, SettingsBase const& settings, FilterResult fres)
{
	m_consumer.baseProcessEvent(evt, prod, settings, fres);
}

void ConsumerBaseAccess::ProcessFilteredEvent(EventBase const& evt, ProductBase const& prod, SettingsBase const& settings)
{
	m_consumer.baseProcessFilteredEvent(evt, prod, settings);
}

void ConsumerBaseAccess::Init(SettingsBase const& settings)
{
	m_consumer.baseInit(settings);
}

void ConsumerBaseAccess::OnLumi(EventBase const& evt, SettingsBase const& settings)
{
	m_consumer.baseOnLumi(evt, settings);
}

void ConsumerBaseAccess::OnRun(EventBase const& evt, SettingsBase const& settings)
{
	m_consumer.baseOnRun(evt, settings);
}

void ConsumerBaseAccess::Finish(SettingsBase const& settings)
{
	m_consumer.baseFinish(settings);
}
