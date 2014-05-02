
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/Utility.h"


/**
   \brief GlobalProducer, for valid electrons.
   
   This producer implements quality criteria for electrons. Currently only the
   *mvanontrig* working point for non-triggering MVA is implemented.

   This Producer needs the following config tags:
     ElectronID
*/

template<class TTypes>
class ValidElectronsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	enum class ElectronID : int
	{
		NONE  = -1,
		MVANONTRIG = 0,
	};
	static ElectronID ToElectronID(std::string const& electronID)
	{
		if (electronID == "mvanontrig")
			return ElectronID::MVANONTRIG;
		else return ElectronID::NONE;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_electrons";
	}

	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		electronID = ToElectronID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(globalSettings.GetElectronID())));
	}

	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		electronID = ToElectronID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetElectronID())));
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}


protected:

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product) const
	{
		for (KDataElectrons::iterator electron = event.m_electrons->begin();
			 electron != event.m_electrons->end(); electron++)
		{
			bool validElectron = true;

			// Electron IDs
			if (electronID == ElectronID::MVANONTRIG)
				validElectron = validElectron && IsMVANonTrigElectron(&(*electron), event, product);
			else if (electronID != ElectronID::NONE)
				LOG(FATAL) << "Electron ID of type " << Utility::ToUnderlyingValue(electronID) << " not yet implemented!";

			// check possible analysis-specific criteria
			validElectron = validElectron && AdditionalCriteria(&(*electron), event, product);

			if (validElectron)
				product.m_validElectrons.push_back(&(*electron));
			else
				product.m_invalidElectrons.push_back(&(*electron));
		}
	}

	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataElectron* electron, event_type const& event, product_type& product) const
	{
		bool validElectron = true;
		return validElectron;
	}


private:
	ElectronID electronID;

	bool IsMVANonTrigElectron(KDataElectron* electron, event_type const& event, product_type& product) const
	{
		bool validElectron = true;

		// Electron ID mva non trig
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Non_triggering_MVA
		// different thresholds depending on electron pT, eta
		validElectron = validElectron &&
			(
				(
					(electron->p4.Pt() < 10)
					&&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaNonTrigV0 > 0.47)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < 1.479 && electron->idMvaNonTrigV0 > 0.004)
						|| (abs(electron->p4.Eta()) > 1.479 && abs(electron->p4.Eta()) < 2.5 && electron->idMvaNonTrigV0 > 0.295)
					)
				)
				||
				(
					(electron->p4.Pt() > 10) &&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaNonTrigV0 > -0.34)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < 1.479 && electron->idMvaNonTrigV0 > -0.65)
						|| (abs(electron->p4.Eta()) > 1.479 && abs(electron->p4.Eta()) < 2.5 && electron->idMvaNonTrigV0 > 0.6)
					)
				)
			);
		validElectron = validElectron
						&& electron->track.nInnerHits <= 1
						&& (electron->trackIso04 / electron->p4.Pt()) < 0.4;

		return validElectron;
	}
};

