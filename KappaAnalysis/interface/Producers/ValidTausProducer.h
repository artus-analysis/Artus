
#pragma once

#include <algorithm>

#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief GlobalProducer, for valid taus.
   
   Required config tags in addtion to the ones of the base class:
   - TauDiscriminators
   - TauLowerPtCuts
   - TauUpperAbsEtaCuts
*/

template<class TTypes>
class ValidTausProducer: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, KDataPFTau>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_taus";
	}
	
	ValidTausProducer() :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, KDataPFTau>(&setting_type::GetTauLowerPtCuts,
		                                            &setting_type::GetTauUpperAbsEtaCuts,
		                                            &product_type::m_validTaus)
	{
	}
	
	virtual void Init(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		ValidPhysicsObjectTools<TTypes, KDataPFTau>::Init(settings);
	
		// parse additional config tags
		discriminatorsByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap(settings.GetTauDiscriminators()),
		                                                                    discriminatorsByHltName);
		
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::Quantities["nTaus"] = [](event_type const& event, product_type const& product) {
			return product.m_validTaus.size();
		};
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (KDataPFTaus::iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); tau++)
		{
			bool validTau = true;
			
			// check discriminators
			for (std::map<size_t, std::vector<std::string> >::const_iterator discriminatorByIndex = discriminatorsByIndex.begin();
				 validTau && (discriminatorByIndex != discriminatorsByIndex.end()); ++discriminatorByIndex)
			{
				if (discriminatorByIndex->first == product.m_validTaus.size())
				{
					validTau = validTau && ApplyDiscriminators(&(*tau), discriminatorByIndex->second, event);
				}
			}
			
			for (std::map<std::string, std::vector<std::string> >::const_iterator discriminatorByHltName = discriminatorsByHltName.begin();
				 validTau && (discriminatorByHltName != discriminatorsByHltName.end()); ++discriminatorByHltName)
			{
				if ((discriminatorByHltName->first == "default") ||
					boost::regex_search(product.m_selectedHltName, boost::regex(discriminatorByHltName->first, boost::regex::icase | boost::regex::extended)))
				{
					validTau = validTau && ApplyDiscriminators(&(*tau), discriminatorByHltName->second, event);
				}
			}
			
			// kinematic cuts
			validTau = validTau && this->PassKinematicCuts(&(*tau), event, product);
			
			// check possible analysis-specific criteria
			validTau = validTau && AdditionalCriteria(&(*tau), event, product, settings);
			
			if (validTau)
			{
				product.m_validTaus.push_back(&(*tau));
				product.m_validLeptons.push_back(&(*tau));
			}
			else
			{
				product.m_invalidTaus.push_back(&(*tau));
				product.m_invalidLeptons.push_back(&(*tau));
			}
		}
		
		// sort vectors of leptons by pt
		std::sort(product.m_validLeptons.begin(), product.m_validLeptons.end(),
		          [](KLepton const* lepton1, KLepton const* lepton2) -> bool
		          { return lepton1->p4.Pt() > lepton2->p4.Pt(); });
		std::sort(product.m_invalidLeptons.begin(), product.m_invalidLeptons.end(),
		          [](KLepton const* lepton1, KLepton const* lepton2) -> bool
		          { return lepton1->p4.Pt() > lepton2->p4.Pt(); });
	}


protected:
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataPFTau* tau, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validTau = true;
		return validTau;
	}


private:
	std::map<size_t, std::vector<std::string> > discriminatorsByIndex;
	std::map<std::string, std::vector<std::string> > discriminatorsByHltName;
	
	bool ApplyDiscriminators(KDataPFTau* tau, std::vector<std::string> const& discriminators,
	                         event_type const& event) const
	{
		bool validTau = true;
		
		for (std::vector<std::string>::const_iterator discriminator = discriminators.begin();
		     validTau && (discriminator != discriminators.end()); ++discriminator)
		{
			validTau = validTau && tau->hasID(*discriminator, event.m_tauDiscriminatorMetadata);
		}
		
		return validTau;
	}

};

