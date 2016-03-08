
#pragma once

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <TMVA/Reader.h>
#include <TString.h>

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Abstract producer base for multi reading/applying TMVA classifications.
*/
template<class TTypes>
class TmvaClassificationMultiReaderBase: public ProducerBase<TTypes>
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
	
	TmvaClassificationMultiReaderBase(std::vector<std::string>& (setting_type::*GetTmvaInputQuantities)(void) const,
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

	void Init(setting_type const& settings) override
	{
		ProducerBase<TTypes>::Init(settings);
		
		// construct extractors vector
		for (std::vector<std::string>::const_iterator quantity_str = (settings.*GetTmvaInputQuantities)().begin();
			 quantity_str != (settings.*GetTmvaInputQuantities)().end(); ++quantity_str)
		{
			//tmvaReader.emplace_back(new TMVA::Reader());
			tmvaReader.push_back(std::make_shared<TMVA::Reader>());
			m_inputExtractors.push_back(std::vector<float_extractor_lambda>());
			LOG(WARNING) << "Parse QuantitiyString " << std::endl << *quantity_str;
			
			std::vector<std::string> quantity_number;
			boost::algorithm::split(quantity_number, *quantity_str, boost::algorithm::is_any_of(";"));
			transform(quantity_number.begin(), quantity_number.end(), quantity_number.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			int input_index = boost::lexical_cast<int>(quantity_number.front());
			LOG(WARNING) << "Current InputIndex for InputExtractor is " << input_index;
			
			std::vector<std::string> quantities_vec;
			boost::algorithm::split(quantities_vec, quantity_number.back(), boost::algorithm::is_any_of(","));
			transform(quantities_vec.begin(), quantities_vec.end(), quantities_vec.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			
			m_inputExtractors[input_index].clear();
			for (std::vector<std::string>::const_iterator quantity = quantities_vec.begin();
			 quantity != quantities_vec.end(); ++quantity)
			{
				std::vector<std::string> splitted;
				boost::algorithm::split(splitted, *quantity, boost::algorithm::is_any_of(":="));
				transform(splitted.begin(), splitted.end(), splitted.begin(),
						[](std::string s) { return boost::algorithm::trim_copy(s); });
				std::string lambdaQuantity = splitted.front();
				LOG(WARNING) << "Find lambdaQuantity: " << lambdaQuantity;
				if (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(lambdaQuantity) > 0)
				{
					m_inputExtractors[input_index].push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetFloatQuantities(), lambdaQuantity));
				}
				else if(LambdaNtupleConsumer<TTypes>::GetIntQuantities().count(lambdaQuantity) > 0)
				{
					m_inputExtractors[input_index].push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetIntQuantities(), lambdaQuantity));
				}
				else
				{	
					LOG(FATAL) << "The TMVA interface currently only supports float-type and int-type input variables!";
				}
				
				// register TMVA input variables
				tmvaReader[input_index]->AddVariable(*quantity, static_cast<float*>(nullptr));
			}
		}
		
// 		for (std::vector<std::string>::const_iterator quantity = (settings.*GetTmvaInputQuantities)().begin();
// 			 quantity != (settings.*GetTmvaInputQuantities)().end(); ++quantity)
// 		{
// 			tmvaReader.AddVariable(*quantity, static_cast<float*>(nullptr));
// 		}
		
		// loading TMVA weight files
		assert((settings.*GetTmvaMethods)().size() == (settings.*GetTmvaWeights)().size());
		LOG(DEBUG) << "\tLoading TMVA weight files...";
		int mvaMethodIndex = 0;
		for(std::vector<std::string>::const_iterator method_str = (settings.*GetTmvaMethods)().begin();
			 method_str != (settings.*GetTmvaMethods)().end(); ++method_str)
		{	
			std::vector<std::string> method_splits;
			boost::algorithm::split(method_splits, *method_str, boost::algorithm::is_any_of(";"));
			transform(method_splits.begin(), method_splits.end(), method_splits.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			int input_index = boost::lexical_cast<int>(method_splits.front());
			
			std::string tmvaMethod = boost::lexical_cast<std::string>(input_index) + method_splits.back() + boost::lexical_cast<std::string>(mvaMethodIndex);
			std::string tmvaWeights = (settings.*GetTmvaWeights)()[mvaMethodIndex];
			LOG(DEBUG) << "\t\tmethod: " << tmvaMethod << ", weight file: " << tmvaWeights;
			tmvaReader[input_index]->BookMVA(tmvaMethod, tmvaWeights);
			mvaMethodIndex += 1;
		}
	}

	void Produce(event_type const& event, product_type& product,
						 setting_type const& settings) const override
	{
		// construct and fill input vector + retrieve outputs
		int mvaMethodIndex = 0;
		(product.*m_mvaOutputsMember) = std::vector<double>((settings.*GetTmvaMethods)().size());
		for(std::vector<std::string>::const_iterator method_str = (settings.*GetTmvaMethods)().begin();
			 method_str != (settings.*GetTmvaMethods)().end(); ++method_str)
		{	
			std::vector<std::string> method_splits;
			boost::algorithm::split(method_splits, *method_str, boost::algorithm::is_any_of(";"));
			transform(method_splits.begin(), method_splits.end(), method_splits.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			int input_index = boost::lexical_cast<int>(method_splits.front().c_str());
			
			std::vector<float_extractor_lambda> Extractor_vec = m_inputExtractors[input_index]; 
			std::vector<double> tmvaInputs(Extractor_vec.size());
			size_t inputQuantityIndex = 0;
			for(typename std::vector<float_extractor_lambda>::const_iterator inputExtractor = Extractor_vec.begin();
				inputExtractor != Extractor_vec.end(); ++inputExtractor)
			{
				tmvaInputs[inputQuantityIndex] = (*inputExtractor)(event, product);
				++inputQuantityIndex;
			}
			
			
			std::string tmvaMethod = boost::lexical_cast<std::string>(input_index) + boost::lexical_cast<std::string>(method_splits.back()) + boost::lexical_cast<std::string>(mvaMethodIndex);
			const TString const_Method = TString(tmvaMethod.c_str());
			const std::vector<double> const_tmvaInputs = tmvaInputs;
			(product.*m_mvaOutputsMember)[mvaMethodIndex] = tmvaReader[input_index]->EvaluateMVA(const_tmvaInputs, const_Method);
			
			mvaMethodIndex += 1; 
		}
	}


private:
	std::vector<std::string>& (setting_type::*GetTmvaInputQuantities)(void) const;
	std::vector<std::string>& (setting_type::*GetTmvaMethods)(void) const;
	std::vector<std::string>& (setting_type::*GetTmvaWeights)(void) const;
	std::vector<double> product_type::*m_mvaOutputsMember;
	
	std::vector<std::vector<float_extractor_lambda>> m_inputExtractors;
	std::vector<std::shared_ptr<TMVA::Reader>> tmvaReader;

};


/**
   \brief Producer for general MVA discriminators
   
   Required config tags:
   - TmvaInputQuantities
   - TmvaMethods
   - TmvaWeights (same length as for TmvaMethods required)
*/
class MultiTmvaClassificationReader: public TmvaClassificationMultiReaderBase<KappaTypes>
{
public:

	std::string GetProducerId() const override;
	
	MultiTmvaClassificationReader();
	
};
