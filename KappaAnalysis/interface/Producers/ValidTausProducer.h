
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

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
	};

	static TauID ToTauID(std::string const& tauIDMethod)
	{
		if(tauIDMethod == "TauIDRecommendation13TeV") return TauID::RECOMMENDATION13TEV;
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
		ValidPhysicsObjectTools<KappaTypes, KTau>(&KappaSettings::GetTauLowerPtCuts,
		                                    &KappaSettings::GetTauUpperAbsEtaCuts,
		                                    &KappaProduct::m_validTaus),
		tauID(TauID::NONE)
	{
	}
	
	void Init(KappaSettings const& settings)  override
	{
		KappaProducerBase::Init(settings);
		ValidPhysicsObjectTools<KappaTypes, KTau>::Init(settings);
		
		validTausInput = ToValidTausInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidTausInput())));
	
		// parse additional config tags
		discriminatorsByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap(settings.GetTauDiscriminators()),
		                                                                    discriminatorsByHltName);
		tauID = ToTauID(settings.GetTauID());
		oldTauDMs = settings.GetTauUseOldDMs();

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nTaus", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size();
		} );
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4 : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->p4.mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauCharge", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->charge() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauSumChargedHadronsLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumChargedHadronsMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->sumChargedHadronCandidates().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingTauSumNeutralHadronsLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingTauSumNeutralHadronsMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 1 ? product.m_validTaus[0]->piZeroMomentum().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4 : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauCharge", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->charge() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauSumChargedHadronsLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumChargedHadronsMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->sumChargedHadronCandidates().mass() : DefaultValues::UndefinedFloat;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingTauSumNeutralHadronsLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum() : DefaultValues::UndefinedRMFLV;
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingTauSumNeutralHadronsMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validTaus.size() >= 2 ? product.m_validTaus[1]->piZeroMomentum().mass() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
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
			// kinematic cuts
			validTau = validTau && this->PassKinematicCuts(*tau, event, product);
			
			// check possible analysis-specific criteria
			validTau = validTau && AdditionalCriteria(*tau, event, product, settings);
			
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
	virtual bool AdditionalCriteria(KTau* tau, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
		bool validTau = true;
		return validTau;
	}


private:
	ValidTausInput validTausInput;
	
	std::map<size_t, std::vector<std::string> > discriminatorsByIndex;
	std::map<std::string, std::vector<std::string> > discriminatorsByHltName;
	
	bool ApplyDiscriminators(KTau* tau, std::vector<std::string> const& discriminators,
	                         KappaEvent const& event) const
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

	bool IsTauIDRecommendation13TeV(KTau* tau, KappaEvent const& event, bool const& oldTauDMs) const
	{
		const KVertex* vertex = new KVertex(event.m_vertexSummary->pv);
		float decayModeDiscriminator = (oldTauDMs ? tau->getDiscriminator("decayModeFinding", event.m_tauMetadata)
							  : tau->getDiscriminator("decayModeFindingNewDMs", event.m_tauMetadata));
		return ( decayModeDiscriminator > 0.5
			 && (std::abs(tau->track.ref.z() - vertex->position.z()) < 0.2)
			// tau dZ requirement for Phys14 sync
			//&& (Utility::ApproxEqual(tau->track.ref.z(), vertex->position.z()))
		);
	}
};

