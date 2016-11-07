/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <sstream>

#include "Artus/Core/interface/ProcessNodeBase.h"
#include "Artus/Utility/interface/ArtusLogging.h"

#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/EventBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"

class FilterBaseAccess;

class FilterBaseUntemplated : public ProcessNodeBase {
public:

	// this will allow the pipeline class to call the protected
	// base implemenations
	friend FilterBaseAccess;

	virtual ~FilterBaseUntemplated();

	/*
	 * Must return a unique id of the filter.
	 */
	virtual std::string GetFilterId() const;

protected:

	virtual bool baseDoesEventPass(EventBase const& event,
			ProductBase const& product, SettingsBase const& settings) const = 0;
	virtual void baseOnRun(EventBase const& event, SettingsBase const& settings) = 0;
	virtual void baseOnLumi(EventBase const& event, SettingsBase const& settings) = 0;
	virtual void baseInit ( SettingsBase const& settings ) = 0;

};

class FilterBaseAccess  {
public:
	explicit FilterBaseAccess(FilterBaseUntemplated& cb) : m_cb(cb) {}

	bool DoesEventPass(EventBase const& event,
			ProductBase const& product, SettingsBase const& settings) const
	{
		return m_cb.baseDoesEventPass( event, product, settings);
	}

	void Init ( SettingsBase const& settings )
	{
		m_cb.baseInit( settings );
	}

	void OnLumi(EventBase const& event, SettingsBase const& settings) const
	{
		m_cb.baseOnLumi( event, settings);
	}

	void OnRun(EventBase const& event, SettingsBase const& settings) const
	{
		m_cb.baseOnRun( event, settings);
	}

private:
	FilterBaseUntemplated & m_cb;
};

template<class TTypes>
class FilterBase: public FilterBaseUntemplated {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual ~FilterBase() {
	}

	// initialise global pre-filters
	virtual void Init(setting_type const& settings)
	{
		LOG(DEBUG) << "Initialize filter \"" << this->GetFilterId() << "\".";
	}

	// process global event
	virtual bool DoesEventPass(event_type const& event,
			product_type const& product, setting_type const& settings) const
	{
		LOG(FATAL) << "DoesEventPassGlobal function for filter \"" << this->GetFilterId() << "\" is not implemented!";
		return false;
	}
	virtual void OnLumi(event_type const& event, setting_type const& settings){ return; }
	virtual void OnRun(event_type const& event, setting_type const& settings){ return; }

	virtual std::string ToString(bool bVerbose = false) {
		return GetFilterId();
	}

	ProcessNodeType GetProcessNodeType () const final
	{
		return ProcessNodeType::Filter;
	}

protected:

	bool baseDoesEventPass(EventBase const& evt,
	                       ProductBase const& prod,
	                       SettingsBase const& settings ) const override {
		auto const& specEvent = static_cast < event_type const&> ( evt );
		auto const& specProd = static_cast < product_type const&> ( prod );
		auto const& specSetting = static_cast < setting_type const&> ( settings );

		return DoesEventPass( specEvent, specProd, specSetting );
	}

	void baseInit (SettingsBase const& settings) override {
		auto const& specSettings = static_cast < setting_type const&> ( settings );

		this->Init ( specSettings );
	}

	void baseOnLumi(EventBase const& evt, SettingsBase const& settings ) override {
		auto const& specEvent = static_cast < event_type const&> ( evt );
		auto const& specSetting = static_cast < setting_type const&> ( settings );

		OnLumi( specEvent, specSetting );
	}

	void baseOnRun(EventBase const& evt, SettingsBase const& settings ) override {
		auto const& specEvent = static_cast < event_type const&> ( evt );
		auto const& specSetting = static_cast < setting_type const&> ( settings );

		OnRun( specEvent, specSetting );
	}

};
