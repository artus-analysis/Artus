
#include "Artus/Core/interface/FilterBase.h"


FilterBaseUntemplated::~FilterBaseUntemplated()
{
}

std::string FilterBaseUntemplated::GetFilterId() const
{
	return GetId();
}


FilterBaseAccess::FilterBaseAccess(FilterBaseUntemplated& cb) :
		m_cb(cb)
{
}

bool FilterBaseAccess::DoesEventPass(EventBase const& event,
		ProductBase const& product, SettingsBase const& settings) const
{
	return m_cb.baseDoesEventPass(event, product, settings);
}

void FilterBaseAccess::Init(SettingsBase const& settings)
{
	m_cb.baseInit(settings);
}

void FilterBaseAccess::OnLumi(EventBase const& event, SettingsBase const& settings) const
{
	m_cb.baseOnLumi(event, settings);
}

void FilterBaseAccess::OnRun(EventBase const& event, SettingsBase const& settings) const
{
	m_cb.baseOnRun(event, settings);
}

