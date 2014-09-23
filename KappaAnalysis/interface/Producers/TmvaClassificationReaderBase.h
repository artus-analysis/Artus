
#pragma once

#include "TMVA/Reader.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Abstract producer base for reading/applying TMVA classifications.
*/
template<class TTypes>
class TmvaClassificationReaderBase: public ProducerBase<TTypes>
{

public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda;
	
	static double GetMvaOutput(std::string const& methodName, std::vector<double> const& mvaOutputs)
	{
		auto methodNameIndex = std::find(mvaOutputs.begin(), mvaOutputs.end(), methodName);
		return (methodNameIndex == mvaOutputs.end() ? DefaultValues::UndefinedDouble : mvaOutputs[methodNameIndex - mvaOutputs.begin()]);
	}
	
	TmvaClassificationReaderBase(std::vector<std::string>& (setting_type::*GetTmvaInputQuantities)(void) const,
	                             std::vector<std::string>& (setting_type::*GetTmvaMethods)(void) const,
	                             std::vector<std::string>& (setting_type::*GetTmvaWeights)(void) const,
	                             std::vector<double> product_type::*mvaOutputs) :
		ProducerBase<TTypes>(),
		GetTmvaInputQuantities(GetTmvaInputQuantities),
		GetTmvaMethods(GetTmvaMethods),
		GetTmvaWeights(GetTmvaWeights),
		m_mvaOutputsMember(mvaOutputs)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		// construct extractors vector
		m_inputExtractors.clear();
		for (std::vector<std::string>::const_iterator quantity = (settings.*GetTmvaInputQuantities)().begin();
		     quantity != (settings.*GetTmvaInputQuantities)().end(); ++quantity)
		{
			m_inputExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetQuantities(), *quantity));
		}
		
		// loading TMVA weight files
		assert((settings.*GetTmvaMethods)().size() == (settings.*GetTmvaWeights)().size());
		LOG(DEBUG) << "\tLoading TMVA weight files...";
		for (size_t mvaMethodIndex = 0; mvaMethodIndex < (settings.*GetTmvaMethods)().size(); ++mvaMethodIndex)
		{
			std::string tmvaMethod = (settings.*GetTmvaMethods)()[mvaMethodIndex];
			std::string tmvaWeights = (settings.*GetTmvaWeights)()[mvaMethodIndex];
			LOG(DEBUG) << "\t\tmethod: " << tmvaMethod << ", weight file: " << tmvaWeights;
			tmvaReader.BookMVA(tmvaMethod, tmvaWeights);
		}
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		// construct and fill input vector
		std::vector<double> tmvaInputs(m_inputExtractors.size());
		size_t inputQuantityIndex = 0;
		for(typename std::vector<float_extractor_lambda>::const_iterator inputExtractor = m_inputExtractors.begin();
		    inputExtractor != m_inputExtractors.end(); ++inputExtractor)
		{
			tmvaInputs[inputQuantityIndex] = (*inputExtractor)(event, product);
			++inputQuantityIndex;
		}
		
		// retrieve MVA outputs
		(product.*m_mvaOutputsMember) = std::vector<double>((settings.*GetTmvaMethods)().size());
		size_t mvaMethodIndex = 0;
		for (std::vector<std::string>::const_iterator tmvaMethod = (settings.*GetTmvaMethods)().begin();
		     tmvaMethod != (settings.*GetTmvaMethods)().end(); ++tmvaMethod)
		{
			(product.*m_mvaOutputsMember)[mvaMethodIndex] = tmvaReader.EvaluateMVA(tmvaInputs, tmvaMethod->c_str());
			++mvaMethodIndex;
		}
	}


private:
	std::vector<std::string>& (setting_type::*GetTmvaInputQuantities)(void) const;
	std::vector<std::string>& (setting_type::*GetTmvaMethods)(void) const;
	std::vector<std::string>& (setting_type::*GetTmvaWeights)(void) const;
	std::vector<double> product_type::*m_mvaOutputsMember;
	
	std::vector<float_extractor_lambda> m_inputExtractors;
	mutable TMVA::Reader tmvaReader;

};


/**
   \brief Producer for general MVA discriminators
   
   Required config tags:
   - TmvaInputQuantities
   - TmvaMethods
   - TmvaWeights (same length as for TmvaMethods required)
*/
class GeneralTmvaClassificationReader: public TmvaClassificationReaderBase<KappaTypes>
{
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE
	{
		return "GeneralTmvaClassificationReader";
	}
	
	GeneralTmvaClassificationReader() :
		TmvaClassificationReaderBase(&KappaSettings::GetTmvaInputQuantities,
		                             &KappaSettings::GetTmvaMethods,
		                             &KappaSettings::GetTmvaWeights,
		                             &KappaProduct::m_discriminators)
	{
	}
	
};
