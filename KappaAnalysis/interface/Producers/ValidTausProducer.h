
#pragma once

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/SafeMap.h"

/**
   \brief GlobalProducer, for valid taus.
   
   Required config tags in addtion to the ones of the base class:
   - TauDiscriminators
*/

template<class TTypes>
class ValidTausProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_taus";
	}
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitGlobal(globalSettings);
	
		// parse additional config tags
		discriminators = ValidTausProducer::ParseTauDiscriminators(globalSettings.GetTauDiscriminators());
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitLocal(settings);
	
		// parse additional config tags
		discriminators = ValidTausProducer::ParseTauDiscriminators(settings.GetTauDiscriminators());
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

	static std::map<int, std::vector<std::string> > ParseTauDiscriminators(std::vector<std::string> discriminatorsToParse)
	{
		std::vector<std::string> defaultDiscriminators;
		std::map<int, std::vector<std::string> > parsedDiscriminators;
	
		for (std::vector<std::string>::iterator discriminator = discriminatorsToParse.begin();
			 discriminator != discriminatorsToParse.end(); ++discriminator)
		{
			std::vector<std::string> splitted;
			boost::algorithm::split(splitted, *discriminator, boost::algorithm::is_any_of(":"));
			transform(splitted.begin(), splitted.end(), splitted.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
		
			int index = -1;
			if (splitted.size() > 1) {
				index = std::stoi(splitted[0]);
			}
		
			if (parsedDiscriminators.count(index) == 0) {
				parsedDiscriminators[index] = std::vector<std::string>();
			}
			parsedDiscriminators[index].push_back(splitted[1]);
		}
	
		return parsedDiscriminators;
	}


protected:

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product) const
	{
		for (KDataPFTaus::iterator tau = event.m_taus->begin();
			 tau != event.m_taus->end(); tau++)
		{
			bool validTau = true;
	
			// get pt-dependent discriminators
			int index = product.m_validTaus.size();
			std::vector<std::string> discriminatorNames = SafeMap::GetWithDefault(discriminators, index,
					                                      SafeMap::GetWithDefault(discriminators, -1, std::vector<std::string>()));
	
			// check discriminators
			for (std::vector<std::string>::iterator discriminatorName = discriminatorNames.begin();
				 validTau && discriminatorName != discriminatorNames.end(); ++discriminatorName)
			{
				validTau = validTau && tau->hasID(*discriminatorName, event.m_tauDiscriminatorMetadata);
			}
			
			// check possible analysis-specific criteria
			validTau = validTau && AdditionalCriteria(&(*tau), event, product);
			
			if (validTau)
				product.m_validTaus.push_back(&(*tau));
			else
				product.m_invalidTaus.push_back(&(*tau));
		}
	}
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KDataPFTau* tau, event_type const& event, product_type& product) const
	{
		bool validTau = true;
		return validTau;
	}


private:
	std::map<int, std::vector<std::string> > discriminators;
};

