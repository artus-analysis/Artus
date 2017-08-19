
#include "Artus/Core/interface/FilterBase.h"


FilterBaseUntemplated::~FilterBaseUntemplated()
{
}


FilterBaseAccess::FilterBaseAccess(FilterBaseUntemplated& filter) :
	m_filter(filter)
{
}

bool FilterBaseAccess::DoesEventPass(EventBase const& event, ProductBase const& product, SettingsBase const& settings) const
{
	return m_filter.baseDoesEventPass(event, product, settings);
}

void FilterBaseAccess::Init(SettingsBase const& settings)
{
	m_filter.baseInit(settings);
}

void FilterBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings) const
{
	m_filter.baseOnLumi(event, settings);
}

void FilterBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings) const
{
	m_filter.baseOnRun(event, settings);
}

