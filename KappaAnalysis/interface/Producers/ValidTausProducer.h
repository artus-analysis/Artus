
#pragma once

#include <algorithm>

#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief GlobalProducer, for valid taus.
   
   Required config tags in addtion to the ones of the base class:
   - TauDiscriminators
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
		ValidPhysicsObjectTools<TTypes, KDataPFTau>(&product_type::m_validTaus)
	{
	}
	
	virtual void Init(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
	
		// parse additional config tags
		discriminators = Utility::ParseVectorToMap(settings.GetTauDiscriminators());
		
		this->lowerPtCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                 this->lowerPtCutsByHltName);
		this->upperAbsEtaCutsByIndex = Utility::ParseMapTypes<size_t, float>(Utility::ParseVectorToMap(settings.GetElectronLowerPtCuts()),
		                                                                     this->upperAbsEtaCutsByHltName);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (KDataPFTaus::iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); tau++)
		{
			bool validTau = true;
	
			// get pt-dependent discriminators
			std::string index = std::to_string(product.m_validTaus.size());
			std::string defaultIndex("default");
			std::vector<std::string> discriminatorNames = SafeMap::GetWithDefault(discriminators, index,
					                                      SafeMap::GetWithDefault(discriminators, defaultIndex, std::vector<std::string>()));
	
			// check discriminators
			for (std::vector<std::string>::iterator discriminatorName = discriminatorNames.begin();
				 validTau && discriminatorName != discriminatorNames.end(); ++discriminatorName)
			{
				validTau = validTau && tau->hasID(*discriminatorName, event.m_tauDiscriminatorMetadata);
			}
			
			// kinematic cuts
			validTau = validTau && this->PassKinematicCuts(&(*tau), event, product);
			
			// check possible analysis-specific criteria
			validTau = validTau && AdditionalCriteria(&(*tau), event, product, settings);
			
			if (validTau)
				product.m_validTaus.push_back(&(*tau));
			else
				product.m_invalidTaus.push_back(&(*tau));
		}
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
	std::map<std::string, std::vector<std::string> > discriminators;

};

