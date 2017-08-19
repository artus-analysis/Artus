
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief GlobalProducer, for valid taus.
   
   Required config tags in addtion to the ones of the base class:
   - ValidTausInput (default: auto)
   - TauDiscriminators
   - TauLowerPtCuts
   - TauUpperAbsEtaCuts
   - DirectIso
*/
class ValidTausProducer: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, KTau>
{

public:

	enum class ValidTausInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};

	enum class TauID : int
	{
		NONE = 0,
		RECOMMENDATION13TEV = 1,
		RECOMMENDATION13TEVAOD = 2,
	};

	static TauID ToTauID(std::string const& tauIDMethod)
	{
		if(tauIDMethod == "TauIDRecommendation13TeV") return TauID::RECOMMENDATION13TEV;
		else if(tauIDMethod == "TauIDRecommendation13TeVAOD") return TauID::RECOMMENDATION13TEVAOD;
		else return TauID::NONE;
	}

	static ValidTausInput ToValidTausInput(std::string const& validTausInput)
	{
		if (validTausInput == "uncorrected") return ValidTausInput::UNCORRECTED;
		else if (validTausInput == "corrected") return ValidTausInput::CORRECTED;
		else return ValidTausInput::AUTO;
	}

	std::string GetProducerId() const override {
		return "ValidTausProducer";
	}
	
	ValidTausProducer() :
		KappaProducerBase(),
		ValidPhysicsObjectTools<KappaTypes, KTau>(&KappaTypes::setting_type::GetTauLowerPtCuts,
		                                    &KappaTypes::setting_type::GetTauUpperAbsEtaCuts,
		                                    &KappaTypes::product_type::m_validTaus),
		tauID(TauID::NONE)
	{
	}
	
	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata)  override
	{
		KappaProducerBase::Init(settings, metadata);
		ValidPhysicsObjectTools<KappaTypes, KTau>::Init(settings);
		
		validTausInput = ToValidTausInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidTausInput())));
	
		// parse additional config tags
		discriminatorsByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap(settings.GetTauDiscriminators()),
		                                                                    discriminatorsByHltName);
		tauID = ToTauID(settings.GetTauID());
		oldTauDMs = settings.GetTauUseOldDMs();

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nTaus", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size();
		} );
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4 : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("leadingTauDecayMode", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->decayMode : DefaultValues::UndefinedInt;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauCharge", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->charge() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauSumChargedHadronsLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauSumNeutralHadronsLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4 : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("trailingTauDecayMode", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->decayMode : DefaultValues::UndefinedInt;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauCharge", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->charge() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauSumChargedHadronsLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauSumNeutralHadronsLV", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsPt", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsEta", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsPhi", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsMass", [](KappaTypes::event_type const& event, KappaTypes::product_type const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().mass() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
	             KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const override
	{
		assert(event.m_taus);
		assert(event.m_tauMetadata);
	
		// select input source
		std::vector<KTau*> taus;
		if ((validTausInput == ValidTausInput::AUTO && (product.m_correctedTaus.size() > 0)) || (validTausInput == ValidTausInput::CORRECTED))
		{
			taus.resize(product.m_correctedTaus.size());
			size_t tauIndex = 0;
			for (std::vector<std::shared_ptr<KTau> >::iterator tau = product.m_correctedTaus.begin();
			     tau != product.m_correctedTaus.end(); ++tau)
			{
				taus[tauIndex] = tau->get();
				++tauIndex;
			}
		}
		else
		{
			taus.resize(event.m_taus->size());
			size_t tauIndex = 0;
			for (KTaus::iterator tau = event.m_taus->begin(); tau != event.m_taus->end(); ++tau)
			{
				taus[tauIndex] = &(*tau);
				++tauIndex;
			}
		}
		
		for (std::vector<KTau*>::iterator tau = taus.begin(); tau != taus.end(); ++tau)
		{
			bool validTau = true;
			
			// check discriminators
			for (std::map<size_t, std::vector<std::string> >::const_iterator discriminatorByIndex = discriminatorsByIndex.begin();
				 validTau && (discriminatorByIndex != discriminatorsByIndex.end()); ++discriminatorByIndex)
			{
				if (discriminatorByIndex->first == product.m_validTaus.size())
				{
					validTau = validTau && ApplyDiscriminators(*tau, discriminatorByIndex->second, event);
				}
			}
			
			for (std::map<std::string, std::vector<std::string> >::const_iterator discriminatorByHltName = discriminatorsByHltName.begin();
				 validTau && (discriminatorByHltName != discriminatorsByHltName.end()); ++discriminatorByHltName)
			{
				bool hasMatch = false;
				for (unsigned int iHlt = 0; iHlt < product.m_selectedHltNames.size(); ++iHlt)
					hasMatch = hasMatch || boost::regex_search(product.m_selectedHltNames.at(iHlt), boost::regex(discriminatorByHltName->first, boost::regex::icase | boost::regex::extended));

				if ((discriminatorByHltName->first == "default") || hasMatch)
				{
					validTau = validTau && ApplyDiscriminators(*tau, discriminatorByHltName->second, event);
				}
			}
			
			if(tauID == TauID::RECOMMENDATION13TEV)
					validTau = validTau && IsTauIDRecommendation13TeV(*tau, event, oldTauDMs);
			if(tauID == TauID::RECOMMENDATION13TEVAOD)
					validTau = validTau && IsTauIDRecommendation13TeV(*tau, event, oldTauDMs, true);
			// kinematic cuts
			validTau = validTau && this->PassKinematicCuts(*tau, event, product);
			
			// check possible analysis-specific criteria
			validTau = validTau && AdditionalCriteria(*tau, event, product, settings, metadata);
			
			if (validTau)
			{
				product.m_validTaus.push_back(*tau);
			}
			else
			{
				product.m_invalidTaus.push_back(*tau);
			}
		}
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KTau* tau, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const
	{
		bool validTau = true;
		return validTau;
	}


private:
	ValidTausInput validTausInput;
	
	std::map<size_t, std::vector<std::string> > discriminatorsByIndex;
	std::map<std::string, std::vector<std::string> > discriminatorsByHltName;
	
	bool ApplyDiscriminators(KTau* tau, std::vector<std::string> const& discriminators,
	                         KappaTypes::event_type const& event) const
	{
		bool validTau = true;
		
		for (std::vector<std::string>::const_iterator discriminator = discriminators.begin();
		     validTau && (discriminator != discriminators.end()); ++discriminator)
		{
			validTau = validTau && tau->getId(*discriminator, event.m_tauMetadata);
		}
		
		return validTau;
	}

	TauID tauID;
	bool oldTauDMs;

	bool IsTauIDRecommendation13TeV(KTau* tau, KappaTypes::event_type const& event, bool const& oldTauDMs, bool const& isAOD=false) const
	{
		const KVertex* vertex = new KVertex(event.m_vertexSummary->pv);
		float decayModeDiscriminator = (oldTauDMs ? tau->getDiscriminator("decayModeFinding", event.m_tauMetadata)
							  : tau->getDiscriminator("decayModeFindingNewDMs", event.m_tauMetadata));
		if(isAOD)
		{
			return ( decayModeDiscriminator > 0.5
				 && (std::abs(tau->track.ref.z() - vertex->position.z()) < 0.2)
				// tau dZ requirement for Phys14 sync
				//&& (Utility::ApproxEqual(tau->track.ref.z(), vertex->position.z()))
			);
		}
		else
		{
			return ( decayModeDiscriminator > 0.5
				 && std::abs(tau->dz) < 0.2
				// tau dZ requirement for Phys14 sync
				//&& (Utility::ApproxEqual(tau->track.ref.z(), vertex->position.z()))
			);
		}
	}
};

