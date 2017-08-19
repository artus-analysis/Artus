
#include "Artus/Core/interface/FilterBase.h"


FilterBaseUntemplated::~FilterBaseUntemplated()
{
}


FilterBaseAccess::FilterBaseAccess(FilterBaseUntemplated& filter) :
	m_filter(filter)
{
}

void FilterBaseAccess::Init(SettingsBase const& settings, MetadataBase& metadata)
{
	m_filter.baseInit(settings, metadata);
}

void FilterBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) const
{
	m_filter.baseOnLumi(event, settings, metadata);
}

void FilterBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) const
{
	m_filter.baseOnRun(event, settings, metadata);
}

bool FilterBaseAccess::DoesEventPass(EventBase const& event, ProductBase const& product, SettingsBase const& settings, MetadataBase const& metadata) const
{
	return m_filter.baseDoesEventPass(event, product, settings, metadata);
}

