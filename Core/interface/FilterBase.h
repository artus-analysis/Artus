
#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

#include "Artus/Core/interface/ProcessNodeBase.h"
#include "Artus/Utility/interface/ArtusLogging.h"

#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/MetadataBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"


class FilterBaseAccess;

class FilterBaseUntemplated : public ProcessNodeBase
{
public:

	// this will allow the pipeline class to call the protected
	// base implemenations
	friend FilterBaseAccess;

	virtual ~FilterBaseUntemplated();

	/*
	 * Must return a unique id of the filter.
	 */
	virtual std::string GetFilterId() const = 0;

protected:

	virtual void baseInit(SettingsBase const& settings, MetadataBase& metadata) = 0;

	virtual void baseOnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) = 0;
	virtual void baseOnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) = 0;

	virtual bool baseDoesEventPass(EventBase const& event, ProductBase const& product, SettingsBase const& settings, MetadataBase const& metadata) const = 0;

};

class FilterBaseAccess
{
public:
	explicit FilterBaseAccess(FilterBaseUntemplated& filter);

	void Init(SettingsBase const& settings, MetadataBase& metadata);

	void OnLumi(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) const;
	void OnRun(EventBase const& event, SettingsBase const& settings, MetadataBase const& metadata) const;

	bool DoesEventPass(EventBase const& event, ProductBase const& product, SettingsBase const& settings, MetadataBase const& metadata) const;

private:
	FilterBaseUntemplated& m_filter;
};

template<class TTypes>
class FilterBase: public FilterBaseUntemplated
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	virtual ~FilterBase()
	{
	}

	virtual void Init(setting_type const& settings, metadata_type& metadata)
	{
		LOG(DEBUG) << "Initialize filter \"" << this->GetFilterId() << "\".";
	}

	virtual void OnLumi(event_type const& event, setting_type const& settings, metadata_type const& metadata)
	{
	}

	virtual void OnRun(event_type const& event, setting_type const& settings, metadata_type const& metadata)
	{
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) const
	{
		LOG(FATAL) << "DoesEventPassGlobal function for filter \"" << this->GetFilterId() << "\" is not implemented!";
		return false;
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return GetFilterId();
	}

	ProcessNodeType GetProcessNodeType() const final
	{
		return ProcessNodeType::Filter;
	}

protected:

	void baseInit(SettingsBase const& settings, MetadataBase& metadata) override
	{
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type& specMetadata = static_cast<metadata_type&>(metadata);

		this->Init(specSettings, specMetadata);
	}

	void baseOnLumi(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata) override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		OnLumi(specEvent, specSettings, specMetadata);
	}

	void baseOnRun(EventBase const& evt, SettingsBase const& settings, MetadataBase const& metadata) override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		OnRun(specEvent, specSettings, specMetadata);
	}

	bool baseDoesEventPass(EventBase const& evt, ProductBase const& prod, SettingsBase const& settings, MetadataBase const& metadata) const override
	{
		event_type const& specEvent = static_cast<event_type const&>(evt);
		product_type const& specProduct = static_cast<product_type const&>(prod);
		setting_type const& specSettings = static_cast<setting_type const&>(settings);
		metadata_type const& specMetadata = static_cast<metadata_type const&>(metadata);

		return DoesEventPass(specEvent, specProduct, specSettings, specMetadata);
	}

};
