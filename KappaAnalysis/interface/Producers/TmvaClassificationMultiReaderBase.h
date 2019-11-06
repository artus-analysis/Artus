
#pragma once

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <TMVA/Reader.h>
#include "TPluginManager.h"
#include <TString.h>

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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
	typedef typename TTypes::metadata_type metadata_type;
	typedef std::function<float(event_type const&, product_type const&, setting_type const& settings, metadata_type const& metadata)> float_extractor_lambda;

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
	
	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		ProducerBase<TTypes>::Init(settings, metadata);
		#if ROOT_VERSION_CODE > ROOT_VERSION(6,0,0)
			gPluginMgr->AddHandler("TMVA@@MethodBase", ".*_FastBDT.*", "TMVA::MethodFastBDT", "TMVAFastBDT", "MethodFastBDT(TMVA::DataSetInfo&,TString)");
			gPluginMgr->AddHandler("TMVA@@MethodBase", ".*FastBDT.*", "TMVA::MethodFastBDT", "TMVAFastBDT", "MethodFastBDT(TString&,TString&,TMVA::DataSetInfo&,TString&)");
		#else
			gPluginMgr->AddHandler("TMVA@@MethodBase", ".*_FastBDT.*", "TMVA::MethodFastBDT", "TMVAFastBDT", "MethodFastBDT(DataSetInfo&,TString)");
			gPluginMgr->AddHandler("TMVA@@MethodBase", ".*FastBDT.*", "TMVA::MethodFastBDT", "TMVAFastBDT", "MethodFastBDT(TString&,TString&,DataSetInfo&,TString&)");
		#endif
		std::vector<float*> tmvaInput;
		// construct extractors vector
		for (std::vector<std::string>::const_iterator quantity_str = (settings.*GetTmvaInputQuantities)().begin();
			 quantity_str != (settings.*GetTmvaInputQuantities)().end(); ++quantity_str)
		{
			//tmvaReader.emplace_back(new TMVA::Reader());
			tmvaReader.push_back(std::make_shared<TMVA::Reader>());
			m_inputExtractors.push_back(std::vector<float_extractor_lambda>());
			LOG(DEBUG) << "Parse QuantitiyString " << std::endl << *quantity_str;
			std::vector<std::string> quantity_number;
			boost::algorithm::split(quantity_number, *quantity_str, boost::algorithm::is_any_of(";"));
			transform(quantity_number.begin(), quantity_number.end(), quantity_number.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			int input_index = boost::lexical_cast<int>(quantity_number.front());
			LOG(DEBUG) << "Current InputIndex for InputExtractor is " << input_index;
			std::vector<std::string> quantities_vec;
			boost::algorithm::split(quantities_vec, quantity_number.back(), boost::algorithm::is_any_of(","));
			transform(quantities_vec.begin(), quantities_vec.end(), quantities_vec.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			m_inputExtractors[input_index].clear();
			tmvaInput.clear();
			for (std::vector<std::string>::const_iterator quantity = quantities_vec.begin();
			 quantity != quantities_vec.end(); ++quantity)
			{
				std::vector<std::string> splitted;
				boost::algorithm::split(splitted, *quantity, boost::algorithm::is_any_of(":="));
				transform(splitted.begin(), splitted.end(), splitted.begin(),
						[](std::string s) { return boost::algorithm::trim_copy(s); });
				std::string lambdaQuantity = splitted.front();
				LOG(DEBUG) << "Find lambdaQuantity: " << lambdaQuantity;
				if (metadata.m_commonFloatQuantities.count(lambdaQuantity) > 0)
				{
					m_inputExtractors[input_index].push_back(SafeMap::Get(metadata.m_commonFloatQuantities, lambdaQuantity));
				}
				else if(metadata.m_commonIntQuantities.count(lambdaQuantity) > 0)
				{
					m_inputExtractors[input_index].push_back(SafeMap::Get(metadata.m_commonIntQuantities, lambdaQuantity));
				}
				else
				{
					LOG(FATAL) << "The TMVA interface currently only supports float-type and int-type input variables!";
				}
				// register TMVA input variables
				tmvaInput.push_back(new float(0));
				tmvaReader[input_index]->AddVariable(*quantity, tmvaInput.back());
			}
			tmvaInputs.push_back(tmvaInput);
		}
		// loading TMVA weight files
		assert((settings.*GetTmvaMethods)().size() == (settings.*GetTmvaWeights)().size());
		LOG(DEBUG) << "\tLoading TMVA weight files...";
		int mvaMethodIndex = 0;
		for(std::vector<std::string>::const_iterator method_str = (settings.*GetTmvaMethods)().begin();
			 method_str != (settings.*GetTmvaMethods)().end(); ++method_str)
		{
			std::vector<std::string> method_split;
			boost::algorithm::split(method_split, *method_str, boost::algorithm::is_any_of(";"));
			transform(method_split.begin(), method_split.end(), method_split.begin(),
					  [](std::string s) { return boost::algorithm::trim_copy(s); });
			int input_index = boost::lexical_cast<int>(method_split.front());
			std::string tmvaMethod = boost::lexical_cast<std::string>(input_index) + method_split.back() + boost::lexical_cast<std::string>(mvaMethodIndex);
			std::string tmvaWeights = (settings.*GetTmvaWeights)()[mvaMethodIndex];
			LOG(DEBUG) << "\t\tmethod: " << tmvaMethod << ", weight file: " << tmvaWeights;
			tmvaReader[input_index]->BookMVA(tmvaMethod, tmvaWeights);
			method_splits.push_back(method_split);
			input_indices.push_back(boost::lexical_cast<int>(method_split.front()));
			mvaMethodIndex += 1;
		}
	}
	
	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override
	{
		// construct and fill input vector + retrieve outputs
		(product.*m_mvaOutputsMember) = std::vector<double>((settings.*GetTmvaMethods)().size());
		for(size_t input_index = 0; input_index < tmvaInputs.size(); input_index++)
		{
			std::vector<float_extractor_lambda> Extractor_vec = m_inputExtractors[input_index];
			size_t inputQuantityIndex = 0;
			for(typename std::vector<float_extractor_lambda>::const_iterator inputExtractor = Extractor_vec.begin();
				inputExtractor != Extractor_vec.end(); ++inputExtractor)
			{
				*(tmvaInputs[input_index][inputQuantityIndex]) = (*inputExtractor)(event, product, settings, metadata);
				++inputQuantityIndex;
			}
		}
		int mvaMethodIndex = 0;
		for(std::vector<std::string>::const_iterator method_str = (settings.*GetTmvaMethods)().begin();
			 method_str != (settings.*GetTmvaMethods)().end(); ++method_str)
		{
			int input_index = input_indices[mvaMethodIndex];
			std::string tmvaMethod = boost::lexical_cast<std::string>(input_index) + boost::lexical_cast<std::string>(method_splits[mvaMethodIndex].back()) + boost::lexical_cast<std::string>(mvaMethodIndex);
			const TString const_Method = TString(tmvaMethod.c_str());
			//const std::vector<double> const_tmvaInputs = tmvaInputs;
			(product.*m_mvaOutputsMember)[mvaMethodIndex] = tmvaReader[input_index]->EvaluateMVA(const_Method);
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
	std::vector<std::vector<std::string>> method_splits;
	std::vector<int> input_indices;
	std::vector<std::vector<float*>> tmvaInputs;

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
