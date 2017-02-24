#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"


/**
   \brief GlobalProducer, for valid electrons.

   This producer implements quality criteria for electrons. Currently only the
   *mvanontrig* working point for non-triggering MVA is implemented.

   This Producer needs the following config tags:
     ValidElectronsInput (default: auto)
     ElectronID
     ElectronIsoType
     ElectronIso
     ElectronReco				(string) for mva or mvatrig ID, perform an additional cut on 'track.nInnerHits'
     ElectronLowerPtCuts
     ElectronUpperAbsEtaCuts
     DirectIso					(bool, default true) if false, inverse the decision of the isolation criterion
*/

template<class TTypes>
class ValidElectronsProducer: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, KElectron>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	enum class ValidElectronsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	static ValidElectronsInput ToValidElectronsInput(std::string const& validElectronsInput)
	{
		if (validElectronsInput == "uncorrected") return ValidElectronsInput::UNCORRECTED;
		else if (validElectronsInput == "corrected") return ValidElectronsInput::CORRECTED;
		else return ValidElectronsInput::AUTO;
	}

	enum class ElectronID : int
	{
		INVALID = -2,
		NONE  = -1,
		MVANONTRIG = 0,
		MVATRIG = 1,
		VBTF95_VETO = 2,
		VBTF95_LOOSE = 3,
		VBTF95_MEDIUM = 4,
		VBTF95_TIGHT = 5,
		FAKEABLE = 6,
		USER  = 7,
		VETO = 8,
		LOOSE = 9,
		MEDIUM = 10,
		TIGHT = 11,
		VBTF95_LOOSE_RELAXEDVTXCRITERIA = 12,
	};
	static ElectronID ToElectronID(std::string const& electronID)
	{
		if (electronID == "mvanontrig") return ElectronID::MVANONTRIG;
		else if (electronID == "mvatrig") return ElectronID::MVATRIG;
		else if (electronID == "vbft95_veto") return ElectronID::VBTF95_VETO;
		else if (electronID == "vbft95_loose") return ElectronID::VBTF95_LOOSE;
		else if (electronID == "vbft95_loose_relaxedvtxcriteria") return ElectronID::VBTF95_LOOSE_RELAXEDVTXCRITERIA;
		else if (electronID == "vbft95_medium") return ElectronID::VBTF95_MEDIUM;
		else if (electronID == "vbft95_tight") return ElectronID::VBTF95_TIGHT;
		else if (electronID == "fakeable") return ElectronID::FAKEABLE;
		else if (electronID == "user") return ElectronID::USER;
		else if (electronID == "none") return ElectronID::NONE;
		else if (electronID == "veto") return ElectronID::VETO;
		else if (electronID == "loose") return ElectronID::LOOSE;
		else if (electronID == "medium") return ElectronID::MEDIUM;
		else if (electronID == "tight") return ElectronID::TIGHT;
		else
			LOG(FATAL) << "Could not find ElectronID " << electronID << "! If you want the ValidElectronsProducer to use no special ID, use \"none\" as argument."<< std::endl;
		return ElectronID::INVALID;
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
		FAKEABLE = 2,
	};
	static ElectronIso ToElectronIso(std::string const& electronIso)
	{
		if (electronIso == "mvanontrig") return ElectronIso::MVANONTRIG;
		else if (electronIso == "mvatrig") return ElectronIso::MVATRIG;
		else if (electronIso == "fakeable") return ElectronIso::FAKEABLE;
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

	ValidElectronsProducer(std::vector<KElectron*> product_type::*validElectrons=&product_type::m_validElectrons,
	                       std::vector<KElectron*> product_type::*invalidElectrons=&product_type::m_invalidElectrons,
	                       std::string (setting_type::*GetElectronID)(void) const=&setting_type::GetElectronID,
	                       std::string (setting_type::*GetElectronIsoType)(void) const=&setting_type::GetElectronIsoType,
	                       std::string (setting_type::*GetElectronIso)(void) const=&setting_type::GetElectronIso,
	                       std::string (setting_type::*GetElectronReco)(void) const=&setting_type::GetElectronReco,
	                       std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const=&setting_type::GetElectronLowerPtCuts,
	                       std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const=&setting_type::GetElectronUpperAbsEtaCuts) :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, KElectron>(GetLowerPtCuts, GetUpperAbsEtaCuts, validElectrons),
		m_validElectronsMember(validElectrons),
		m_invalidElectronsMember(invalidElectrons),
		GetElectronID(GetElectronID),
		GetElectronIsoType(GetElectronIsoType),
		GetElectronIso(GetElectronIso),
		GetElectronReco(GetElectronReco)
	{
	}

	void Init(setting_type const& settings) override {
		ProducerBase<TTypes>::Init(settings);
		ValidPhysicsObjectTools<TTypes, KElectron>::Init(settings);
		
		validElectronsInput = ToValidElectronsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidElectronsInput())));
		
		electronID = ToElectronID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetElectronID)())));
		electronIsoType = ToElectronIsoType(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetElectronIsoType)())));
		electronIso = ToElectronIso(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetElectronIso)())));
		electronReco = ToElectronReco(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy((settings.*GetElectronReco)())));

		if ((boost::algorithm::contains((settings.*GetElectronID)(), "vbft95")) && (electronIso==ElectronIso::NONE))
		{
			LOG(WARNING) << "ValidElectronsProducer: using cutbased vbft95 ID, but isolation is not set!";
		}

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("nElectrons", [](event_type const& event, product_type const& product) {
			return product.m_validElectrons.size();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingElePt", [](event_type const& event, product_type const& product) {
			return product.m_validElectrons.size() >= 1 ? product.m_validElectrons[0]->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("leadingEleEta", [](event_type const& event, product_type const& product) {
			return product.m_validElectrons.size() >= 1 ? product.m_validElectrons[0]->p4.Eta() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const override
	{
		assert(event.m_electrons);
		assert(event.m_vertexSummary);
		assert(event.m_electronMetadata);
		// select input source
		std::vector<KElectron*> electrons;
		if ((validElectronsInput == ValidElectronsInput::AUTO && (product.m_correctedElectrons.size() > 0)) || (validElectronsInput == ValidElectronsInput::CORRECTED))
		{
			electrons.resize(product.m_correctedElectrons.size());
			size_t electronIndex = 0;
			for (std::vector<std::shared_ptr<KElectron> >::iterator electron = product.m_correctedElectrons.begin();
			     electron != product.m_correctedElectrons.end(); ++electron)
			{
				electrons[electronIndex] = electron->get();
				++electronIndex;
			}
		}
		else
		{
			electrons.resize(event.m_electrons->size());
			size_t electronIndex = 0;
			for (KElectrons::iterator electron = event.m_electrons->begin(); electron != event.m_electrons->end(); ++electron)
			{
				electrons[electronIndex] = &(*electron);
				++electronIndex;
			}
		}

		for (std::vector<KElectron*>::iterator electron = electrons.begin(); electron != electrons.end(); ++electron)
		{
			bool valid = true;

			// POG recommondations
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Non_triggering_MVA
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA

			// Electron IDs
			if (electronID == ElectronID::MVANONTRIG)
				valid = valid && IsMVANonTrigElectron(*electron, event.m_electronMetadata);
			else if (electronID == ElectronID::MVATRIG)
				valid = valid && IsMVATrigElectron(*electron, event.m_electronMetadata);
			else if (electronID == ElectronID::VBTF95_VETO)
				valid = valid && IsVetoVbtf95Electron(*electron, event, product);
			else if (electronID == ElectronID::VBTF95_LOOSE)
				valid = valid && IsLooseVbtf95Electron(*electron, event, product);
			else if (electronID == ElectronID::VBTF95_LOOSE_RELAXEDVTXCRITERIA)
				valid = valid && IsLooseVbtf95ElectronRelaxedVtxCriteria(*electron, event, product);
			else if (electronID == ElectronID::VBTF95_MEDIUM)
				valid = valid && IsMediumVbtf95Electron(*electron, event, product);
			else if (electronID == ElectronID::VBTF95_TIGHT)
				valid = valid && IsTightVbtf95Electron(*electron, event, product);
			else if (electronID == ElectronID::FAKEABLE)
				valid = valid && IsFakeableElectron(*electron, event, product);
			else if (electronID == ElectronID::VETO)
				valid = valid && (*electron)->idVeto();
			else if (electronID == ElectronID::LOOSE)
				valid = valid && (*electron)->idLoose();
			else if (electronID == ElectronID::MEDIUM)
				valid = valid && (*electron)->idMedium();
			else if (electronID == ElectronID::TIGHT)
				valid = valid && (*electron)->idTight();
			else if (electronID != ElectronID::USER && electronID != ElectronID::NONE)
				LOG(FATAL) << "Electron ID of type " << Utility::ToUnderlyingValue(electronID) << " not yet implemented!";

			// Electron Isolation
			if (electronIsoType == ElectronIsoType::PF) {
				if (electronIso == ElectronIso::MVANONTRIG)
					valid = valid && ((((*electron)->trackIso / (*electron)->p4.Pt()) < 0.4f) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (electronIso == ElectronIso::MVATRIG)
					valid = valid && ((((*electron)->trackIso / (*electron)->p4.Pt()) < 0.15f) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
				else if (electronIso == ElectronIso::FAKEABLE)
					valid = valid && IsFakeableElectronIso(*electron, event, product, settings);
				else if (electronIso != ElectronIso::NONE)
					LOG(FATAL) << "Electron isolation of type " << Utility::ToUnderlyingValue(electronIso) << " not yet implemented!";
			}
			else if (electronIsoType != ElectronIsoType::USER && electronIsoType != ElectronIsoType::NONE)
			{
				LOG(FATAL) << "Electron isolation type of type " << Utility::ToUnderlyingValue(electronIsoType) << " not yet implemented!";
			}

			// Electron reconstruction
			if (electronReco == ElectronReco::MVANONTRIG)
				valid = valid && ((*electron)->track.nInnerHits <= 1);
				// && sip is the significance of impact parameter in 3D of the electron GSF track < 4 TODO
			else if (electronReco == ElectronReco::MVATRIG)
				valid = valid && ((*electron)->track.nInnerHits == 0);
			else if (electronReco != ElectronReco::USER && electronReco != ElectronReco::NONE)
			{
				LOG(FATAL) << "Electron reconstruction of type " << Utility::ToUnderlyingValue(electronReco) << " not yet implemented!";
			}

			// conversion veto per default
			valid = valid && (! ((*electron)->electronType & (1 << KElectronType::hasConversionMatch)));

			// kinematic cuts
			valid = valid && this->PassKinematicCuts(*electron, event, product);

			// check possible analysis-specific criteria
			valid = valid && AdditionalCriteria(*electron, event, product, settings);

			if (valid)
				(product.*m_validElectronsMember).push_back(*electron);
			else
				(product.*m_invalidElectronsMember).push_back(*electron);
		}
	}

	static bool IsMVANonTrigElectron(const KElectron* electron, const KElectronMetadata* electronMeta)
	{
		// Electron ID mva non trig (run 1)
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Non_triggering_MVA
		// different thresholds depending on electron pT, eta
		if (electron->p4.Pt() < 10.0f)
		{
			return (
				(std::abs(electron->p4.Eta()) < 0.8f && electron->getId("idMvaNonTrigV0", electronMeta) > 0.47f) ||
				(std::abs(electron->p4.Eta()) > 0.8f && std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->getId("idMvaNonTrigV0", electronMeta) > 0.004f) ||
				(std::abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && std::abs(electron->p4.Eta()) < 2.5f && electron->getId("idMvaNonTrigV0", electronMeta) > 0.295f));
		}
		else if (electron->p4.Pt() >= 10.0f)
		{
			return (
				(std::abs(electron->p4.Eta()) < 0.8f && electron->getId("idMvaNonTrigV0", electronMeta) > -0.34f) ||
				(std::abs(electron->p4.Eta()) > 0.8f && std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->getId("idMvaNonTrigV0", electronMeta) > -0.65f) ||
				(std::abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && std::abs(electron->p4.Eta()) < 2.5f && electron->getId("idMvaNonTrigV0", electronMeta) > 0.6f));
		}
		return false;
	}

	static bool IsMVATrigElectron(const KElectron* electron, const KElectronMetadata* electronMeta)
	{
		// Electron ID mva trig (run 1)
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA
		// different thresholds depending on electron pT, eta
		if (electron->p4.Pt() >= 10.0f && electron->p4.Pt() < 20.0f)
		{
			return (
				(std::abs(electron->p4.Eta()) <= 0.8f && electron->getId("idMvaTrigV0", electronMeta) > 0.0f) ||
				(std::abs(electron->p4.Eta()) > 0.8f && std::abs(electron->p4.Eta()) <= DefaultValues::EtaBorderEB && electron->getId("idMvaTrigV0", electronMeta) > 0.1f) ||
				(std::abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && std::abs(electron->p4.Eta()) <= 2.5f && electron->getId("idMvaTrigV0", electronMeta) > 0.62f));
		}
		else if (electron->p4.Pt() >= 20.0f)
		{
			return (
				(std::abs(electron->p4.Eta()) < 0.8f && electron->getId("idMvaTrigV0", electronMeta) > 0.94f) ||
				(std::abs(electron->p4.Eta()) > 0.8f && std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB && electron->getId("idMvaTrigV0", electronMeta) > 0.85f) ||
				(std::abs(electron->p4.Eta()) > DefaultValues::EtaBorderEB && std::abs(electron->p4.Eta()) < 2.5f && electron->getId("idMvaTrigV0", electronMeta) > 0.92f));
		}
		return false;
	}

	static bool IsVetoVbtf95Electron(const KElectron* electron, event_type const& event, product_type const& product)
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Barrel_Cuts_eta_supercluster_1_4
//			return (std::abs(electron->dEtaIn) < 0.007f) &&
//			       (std::abs(electron->dPhiIn) < 0.8f) &&
//			       (electron->sigmaIetaIeta < 0.01f) &&
//			       (electron->hadronicOverEm < 0.15f) &&
//			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.04f) &&
//			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.2f);
//			       // no isolation
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return (std::abs(electron->dEtaIn) < 0.0152f) &&
			       (std::abs(electron->dPhiIn) < 0.216f) &&
			       (electron->sigmaIetaIeta < 0.0114f) &&
			       (electron->hadronicOverEm < 0.181f) &&
			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.0564f) &&
			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.472f);
			       // no isolation
		}
		else
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return (std::abs(electron->dEtaIn) < 0.01f) &&
//			       (std::abs(electron->dPhiIn) < 0.7f) &&
//			       (electron->sigmaIetaIeta < 0.03f) &&
//			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.04f) &&
//			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.2f);
//			       // no isolation
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return (std::abs(electron->dEtaIn) < 0.0113f) &&
			       (std::abs(electron->dPhiIn) < 0.237f) &&
			       (electron->sigmaIetaIeta < 0.0352f) &&
			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.222f) &&
			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.921f);
			       // no isolation
		}
		return false;
	}

	static bool IsLooseVbtf95Electron(const KElectron* electron, event_type const& event, product_type const& product)
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.007f, 0.15f, 0.01f, 0.12f, 0.02f, 0.2f, 0.05f, 0.15f, 1);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.0105f, 0.115f, 0.0103f, 0.104f, 0.0261f, 0.41f, 0.102f, 0.0893f, 2);
		}
		else
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.009f, 0.1f, 0.03f, 0.1f, 0.02f, 0.2f, 0.05f, 0.15f, 1);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.00814f, 0.182f, 0.0301f, 0.0897f, 0.118f, 0.822f, 0.126f, 0.121f, 1);
		}
		return false;
	}
	static bool IsLooseVbtf95ElectronRelaxedVtxCriteria(const KElectron* electron, event_type const& event, product_type const& product)
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
			return CutBasedID(electron, event, 0.0105f, 0.115f, 0.0103f, 0.104f, 1000000.0f, 1000000.0f, 0.102f, 0.0893f, 2);
		}
		else
		{
			return CutBasedID(electron, event, 0.00814f, 0.182f, 0.0301f, 0.0897f, 1000000.0f, 1000000.0f, 0.126f, 0.121f, 1);
		}
		return false;
	}

	static bool IsMediumVbtf95Electron(const KElectron* electron, event_type const& event, product_type const& product)
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.004f, 0.06f, 0.01f, 0.12f, 0.02f, 0.1f, 0.05f, 0.15f, 1);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.0103f, 0.0336f, 0.0101f, 0.0876f, 0.0118f, 0.373f, 0.0174f, 0.0766f, 2);
		}
		else
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.007f, 0.03f, 0.03f, 0.1f, 0.02f, 0.1f, 0.05f, 0.15f, 1);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.00733f, 0.114f, 0.0283f, 0.0678f, 0.0739f, 0.602f, 0.0898f, 0.0678f, 1);
		}
		return false;
	}

	static bool IsTightVbtf95Electron(const KElectron* electron, event_type const& event, product_type const& product)
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.004f, 0.03f, 0.01f, 0.12f, 0.02f, 0.1f, 0.05f, 0.18f, 0);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.00926f, 0.0336f, 0.0101f, 0.0597f, 0.0111f, 0.0466f, 0.012f, 0.0354f, 2);
		}
		else
		{
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Endcap_Cuts_1_479_eta_superclust
//			return CutBasedID(electron, event, 0.005f, 0.02f, 0.03f, 0.1f, 0.02f, 0.1f, 0.05f, 0.18f, 0);
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Spring15_selection_25ns
			return CutBasedID(electron, event, 0.00724f, 0.0918f, 0.0279f, 0.0615f, 0.0351f, 0.417f, 0.0615f, 0.0646f, 1);
		}
		return false;
	}

	static bool CutBasedID(const KElectron* electron, event_type const& event,
	                        float dEtaIn, float dPhiIn, float sigmaIetaIeta,
	                        float hadronicOverEm, float Dxy, float Dz, float EP,
	                        float pfiso, int missingHits)
	{
		// https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification#Electron_ID_Working_Points
		return (std::abs(electron->dEtaIn) < dEtaIn) &&		//abs(dEtaIn)
		       (std::abs(electron->dPhiIn) < dPhiIn) &&		//abs(dPhiIn)
		       (electron->sigmaIetaIeta < sigmaIetaIeta) &&		//full5x5_sigmaIetaIeta
		       (electron->hadronicOverEm < hadronicOverEm) &&		//hOverE
		       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < Dxy) &&		//abs(d0)
		       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < Dz) &&		//abs(dz)
		       (std::abs(1.0f/(electron->ecalEnergy) - 1.0f/(electron->ecalEnergy/electron->eSuperClusterOverP)) < EP) &&	//ooEmooP
		       (electron->track.nInnerHits <= missingHits);		//expectedMissingInnerHits
		       // no isolation		//relIsoWithEA
		       // no conversion rejection
	}


protected:
	ElectronID electronID;
	ElectronIsoType electronIsoType;
	ElectronIso electronIso;
	ElectronReco electronReco;

	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KElectron* electron, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		return true;
	}


private:
	std::vector<KElectron*> product_type::*m_validElectronsMember;
	std::vector<KElectron*> product_type::*m_invalidElectronsMember;
	std::string (setting_type::*GetElectronID)(void) const;
	std::string (setting_type::*GetElectronIsoType)(void) const;
	std::string (setting_type::*GetElectronIso)(void) const;
	std::string (setting_type::*GetElectronReco)(void) const;

	ValidElectronsInput validElectronsInput;

	bool IsFakeableElectron(KElectron* electron, event_type const& event, product_type& product) const
	{
		if (std::abs(electron->p4.Eta()) < DefaultValues::EtaBorderEB)
		{
			return (std::abs(electron->dEtaIn) < 0.007f) &&
			       (std::abs(electron->dPhiIn) < 0.15f) &&
			       (electron->sigmaIetaIeta < 0.01f) &&
			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.02f) &&
			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.1f);
		}
		else
		{
			return (std::abs(electron->dEtaIn) < 0.009f) &&
			       (std::abs(electron->dPhiIn) < 0.1f) &&
			       (electron->sigmaIetaIeta < 0.03f) &&
			       (std::abs(electron->track.getDxy(&event.m_vertexSummary->pv)) < 0.02f) &&
			       (std::abs(electron->track.getDz(&event.m_vertexSummary->pv)) < 0.1f);
		}
		return false;
	}

	bool IsFakeableElectronIso(KElectron* electron, event_type const& event, product_type& product,  setting_type const& settings) const
	{
		return (((electron->trackIso / electron->p4.Pt()) < 0.2f) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				((electron->ecalIso / electron->p4.Pt()) < 0.2f) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				((electron->hcal1Iso / electron->p4.Pt()) < 0.2f) ? settings.GetDirectIso() : (!settings.GetDirectIso()) &&
				((electron->hcal2Iso / electron->p4.Pt()) < 0.2f) ? settings.GetDirectIso() : (!settings.GetDirectIso()));
	}
};

