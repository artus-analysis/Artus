
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"


/**
   \brief GlobalProducer, for valid electrons.
   
   This producer implements quality criteria for electrons. Currently only the
   *mvanontrig* working point for non-triggering MVA is implemented.

   This Producer needs the following config tags:
     ElectronID
     ElectronIsoType
     ElectronIso
     ElectronReco
     ElectronLowerPtCuts
     ElectronUpperAbsEtaCuts
*/

template<class TTypes>
class ValidElectronsProducer: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, KDataElectron>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class ElectronID : int
	{
		NONE  = -1,
		MVANONTRIG = 0,
		MVATRIG = 1,
		USER  = 2,
	};
	static ElectronID ToElectronID(std::string const& electronID)
	{
		if (electronID == "mvanontrig") return ElectronID::MVANONTRIG;
		else if (electronID == "mvatrig") return ElectronID::MVATRIG;
		else if (electronID == "user") return ElectronID::USER;
		else return ElectronID::NONE;
	}
	
	enum class ElectronIsoType : int
	{
		NONE  = -1,
		PF = 0,
		USER = 1,
	};
	static ElectronIsoType ToElectronIsoType(std::string const& electronIsoType)
	{
		if (electronIsoType == "pf") return ElectronIsoType::PF;
		else if (electronIsoType == "user") return ElectronIsoType::USER;
		else return ElectronIsoType::NONE;
	}
	
	enum class ElectronIso : int
	{
		NONE  = -1,
		MVANONTRIG = 0,
		MVATRIG = 1,
	};
	static ElectronIso ToElectronIso(std::string const& electronIso)
	{
		if (electronIso == "mvanontrig") return ElectronIso::MVANONTRIG;
		else if (electronIso == "mvatrig") return ElectronIso::MVATRIG;
		else return ElectronIso::NONE;
	}
	
	enum class ElectronReco : int
	{
		NONE  = -1,
		MVANONTRIG = 0,
		MVATRIG = 1,
		USER = 2,
	};
	static ElectronReco ToElectronReco(std::string const& electronReco)
	{
		if (electronReco == "mvanontrig") return ElectronReco::MVANONTRIG;
		else if (electronReco == "mvatrig") return ElectronReco::MVATRIG;
		else if (electronReco == "user") return ElectronReco::USER;
		else return ElectronReco::NONE;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_electrons";
	}
	
	ValidElectronsProducer() :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, KDataElectron>(&product_type::m_validElectrons)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::Init(settings);
		
		electronID = ToElectronID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetElectronID())));
		electronIsoType = ToElectronIsoType(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetElectronIsoType())));
		electronIso = ToElectronIso(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetElectronIso())));
		electronReco = ToElectronReco(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetElectronReco())));
		
		this->lowerPtCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                 this->lowerPtCutsByHltName);
		this->upperAbsEtaCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                     this->upperAbsEtaCutsByHltName);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (KDataElectrons::iterator electron = event.m_electrons->begin();
			 electron != event.m_electrons->end(); electron++)
		{
			bool validElectron = true;
			
			// POG recommondations
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Non_triggering_MVA
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA

			// Electron IDs
			if (electronID == ElectronID::MVANONTRIG)
				validElectron = validElectron && IsMVANonTrigElectron(&(*electron));
			else if (electronID == ElectronID::MVATRIG)
				validElectron = validElectron && IsMVATrigElectron(&(*electron));
			else if (electronID != ElectronID::USER && electronID != ElectronID::NONE)
				LOG(FATAL) << "Electron ID of type " << Utility::ToUnderlyingValue(electronID) << " not yet implemented!";

			// Electron Isolation
			if (electronIsoType == ElectronIsoType::PF) {
				if (electronIso == ElectronIso::MVANONTRIG)
					validElectron = validElectron && (electron->trackIso04 / electron->p4.Pt()) < 0.4;
				else if (electronIso == ElectronIso::MVATRIG)
					validElectron = validElectron && (electron->trackIso04 / electron->p4.Pt()) < 0.15;
				else if (electronIso != ElectronIso::NONE)
					LOG(FATAL) << "Electron isolation of type " << Utility::ToUnderlyingValue(electronIso) << " not yet implemented!";
			}
			else if (electronIsoType != ElectronIsoType::USER && electronIsoType != ElectronIsoType::NONE)
				LOG(FATAL) << "Electron isolation type of type " << Utility::ToUnderlyingValue(electronIsoType) << " not yet implemented!";

			// Electron reconstruction
			if (electronReco == ElectronReco::MVANONTRIG) {
				validElectron = validElectron
				                && (electron->track.nInnerHits <= 1);
				                // && sip is the significance of impact parameter in 3D of the electron GSF track < 4 TODO
			}
			else if (electronReco == ElectronReco::MVATRIG) {
				validElectron = validElectron
				                && (electron->track.nInnerHits == 0);
			}
			else if (electronReco != ElectronReco::USER && electronReco != ElectronReco::NONE)
				LOG(FATAL) << "Electron reconstruction of type " << Utility::ToUnderlyingValue(electronReco) << " not yet implemented!";
			
			// conversion veto per default
			validElectron = validElectron && !electron->hasConversionMatch;
			
			// kinematic cuts
			validElectron = validElectron && this->PassKinematicCuts(&(*electron), event, product);
			
			// check possible analysis-specific criteria
			validElectron = validElectron && AdditionalCriteria(&(*electron), event, product, settings);

			if (validElectron)
				product.m_validElectrons.push_back(&(*electron));
			else
				product.m_invalidElectrons.push_back(&(*electron));
		}
	}


protected:
	ElectronID electronID;
	ElectronIsoType electronIsoType;
	ElectronIso electronIso;
	ElectronReco electronReco;

	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataElectron* electron, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validElectron = true;
		return validElectron;
	}


private:

	bool IsMVANonTrigElectron(KDataElectron* electron) const
	{
		bool validElectron = true;

		// Electron ID mva non trig
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Non_triggering_MVA
		// different thresholds depending on electron pT, eta
		validElectron = validElectron &&
			(
				(
					(electron->p4.Pt() < 10.0)
					&&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaNonTrigV0 > 0.47)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->idMvaNonTrigV0 > 0.004)
						|| (abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && abs(electron->p4.Eta()) < 2.5 && electron->idMvaNonTrigV0 > 0.295)
					)
				)
				||
				(
					(electron->p4.Pt() >= 10.0) &&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaNonTrigV0 > -0.34)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->idMvaNonTrigV0 > -0.65)
						|| (abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && abs(electron->p4.Eta()) < 2.5 && electron->idMvaNonTrigV0 > 0.6)
					)
				)
			);

		return validElectron;
	}

	bool IsMVATrigElectron(KDataElectron* electron) const
	{
		bool validElectron = true;

		// Electron ID mva trig
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA
		// different thresholds depending on electron pT, eta
		validElectron = validElectron &&
			(
				(
					(electron->p4.Pt() >= 10.0)
					&&
					(electron->p4.Pt() < 20.0)
					&&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaTrigV0 > 0.0)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->idMvaTrigV0 > 0.1)
						|| (abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && abs(electron->p4.Eta()) < 2.5 && electron->idMvaTrigV0 > 0.62)
					)
				)
				||
				(
					(electron->p4.Pt() >= 20.0) &&
					(
						(abs(electron->p4.Eta()) < 0.8 && electron->idMvaTrigV0 > 0.94)
						|| (abs(electron->p4.Eta()) > 0.8 && abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->idMvaTrigV0 > 0.85)
						|| (abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && abs(electron->p4.Eta()) < 2.5 && electron->idMvaTrigV0 > 0.92)
					)
				)
			);

		return validElectron;
	}

};

