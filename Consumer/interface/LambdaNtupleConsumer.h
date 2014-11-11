
#pragma once

#include <boost/algorithm/string/predicate.hpp>

#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/RootFileHelper.h"

#include <cassert>

#include <TTree.h>

/**
 * Fills TTrees with valueExtractors defined as lambda functions
 * This removes the string operations from its base class
 * This consumer can only be fully initilised in the constructor of an derived class
 * where the map LambdaNtupleConsumer<TTypes>::Quantities is filled with analysis specific code
 */


class LambdaNtupleQuantities {

public:
	static std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> CommonBoolQuantities;
	static std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> CommonIntQuantities;
	static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> CommonFloatQuantities;
	static std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> CommonDoubleQuantities;

};

template<class TTypes>
class LambdaNtupleConsumer: public ConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<bool(EventBase const&, ProductBase const&)> bool_extractor_lambda_base;
	typedef std::function<int(EventBase const&, ProductBase const&)> int_extractor_lambda_base;
	typedef std::function<float(EventBase const&, ProductBase const&)> float_extractor_lambda_base;
	typedef std::function<double(EventBase const&, ProductBase const&)> double_extractor_lambda_base;

	
	static void AddBoolQuantity(std::string const& name,
	                            std::function<bool(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonBoolQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> bool
		{ 
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddIntQuantity(std::string const& name,
	                           std::function<int(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonIntQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> int
		{ 
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddFloatQuantity(std::string const& name,
	                             std::function<float(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonFloatQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> float
		{ 
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddDoubleQuantity(std::string const& name,
	                        std::function<double(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonDoubleQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> double
		{ 
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}

	static std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> & GetBoolQuantities () {
		return LambdaNtupleQuantities::CommonBoolQuantities;
	}
	static std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> & GetIntQuantities () {
		return LambdaNtupleQuantities::CommonIntQuantities;
	}
	static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> & GetFloatQuantities () {
		return LambdaNtupleQuantities::CommonFloatQuantities;
	}
	static std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> & GetDoubleQuantities () {
		return LambdaNtupleQuantities::CommonDoubleQuantities;
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::Init(settings);
		
		// construct value extractors
		m_floatValueExtractors.clear();
		
		size_t quantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			if (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(*quantity) > 0)
			{
				m_floatValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetFloatQuantities(), *quantity));
			}
			else if (LambdaNtupleConsumer<TTypes>::GetIntQuantities().count(*quantity) > 0)
			{
				m_intValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetIntQuantities(), *quantity));
			}
			else if (LambdaNtupleConsumer<TTypes>::GetDoubleQuantities().count(*quantity) > 0)
			{
				m_doubleValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetDoubleQuantities(), *quantity));
			}
			else if (LambdaNtupleConsumer<TTypes>::GetBoolQuantities().count(*quantity) > 0)
			{
				m_boolValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetBoolQuantities(), *quantity));
			}
			else
			{
				LOG(FATAL) << "No lambda expression available for quantity \"" << *quantity << "\"!";
			}
			
			quantityIndex++;
		}
		
		// create tree
		m_tree = new TTree("ntuple", ("Tree for Pipeline \"" + settings.GetName() + "\"").c_str());
		
		// create branches
		m_boolValues.resize(m_boolValueExtractors.size());
		m_intValues.resize(m_intValueExtractors.size());
		m_floatValues.resize(m_floatValueExtractors.size());
		m_doubleValues.resize(m_doubleValueExtractors.size());
		
		size_t boolQuantityIndex = 0;
		size_t intQuantityIndex = 0;
		size_t floatQuantityIndex = 0;
		size_t doubleQuantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			if (LambdaNtupleQuantities::CommonFloatQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_floatValues[floatQuantityIndex]), (*quantity + "/F").c_str());
				floatQuantityIndex++;
			}
			else if (LambdaNtupleQuantities::CommonIntQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_intValues[intQuantityIndex]), (*quantity + "/I").c_str());
				intQuantityIndex++;
			}
			else if (LambdaNtupleQuantities::CommonDoubleQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_doubleValues[doubleQuantityIndex]), (*quantity + "/D").c_str());
				doubleQuantityIndex++;
			}
			else if (LambdaNtupleQuantities::CommonBoolQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_boolValues[boolQuantityIndex]), (*quantity + "/O").c_str());
				boolQuantityIndex++;
			}
		}
	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings ) ARTUS_CPP11_OVERRIDE
	{
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings);
		
		// calculate values
		size_t boolValueIndex = 0;
		for(typename std::vector<bool_extractor_lambda_base>::iterator valueExtractor = m_boolValueExtractors.begin();
		    valueExtractor != m_boolValueExtractors.end(); ++valueExtractor)
		{
			m_boolValues[boolValueIndex] = (*valueExtractor)(event, product);
			boolValueIndex++;
		}
		
		size_t intValueIndex = 0;
		for(typename std::vector<int_extractor_lambda_base>::iterator valueExtractor = m_intValueExtractors.begin();
		    valueExtractor != m_intValueExtractors.end(); ++valueExtractor)
		{
			m_intValues[intValueIndex] = (*valueExtractor)(event, product);
			intValueIndex++;
		}
		
		size_t floatValueIndex = 0;
		for(typename std::vector<float_extractor_lambda_base>::iterator valueExtractor = m_floatValueExtractors.begin();
		    valueExtractor != m_floatValueExtractors.end(); ++valueExtractor)
		{
			m_floatValues[floatValueIndex] = (*valueExtractor)(event, product);
			floatValueIndex++;
		}
		
		size_t doubleValueIndex = 0;
		for(typename std::vector<double_extractor_lambda_base>::iterator valueExtractor = m_doubleValueExtractors.begin();
		    valueExtractor != m_doubleValueExtractors.end(); ++valueExtractor)
		{
			m_doubleValues[doubleValueIndex] = (*valueExtractor)(event, product);
			doubleValueIndex++;
		}

		// fill tree
		this->m_tree->Fill();
	}

	virtual void Finish(setting_type const& setting) ARTUS_CPP11_OVERRIDE
	{
		RootFileHelper::SafeCd(setting.GetRootOutFile(), setting.GetRootFileFolder());
		m_tree->Write(m_tree->GetName());
	}


private:
	TTree* m_tree = 0;
	
	std::vector<bool_extractor_lambda_base> m_boolValueExtractors;
	std::vector<int_extractor_lambda_base> m_intValueExtractors;
	std::vector<float_extractor_lambda_base> m_floatValueExtractors;
	std::vector<double_extractor_lambda_base> m_doubleValueExtractors;
	
	std::vector<char> m_boolValues; // needs to be char vector because of bitset treatment of bool vector
	std::vector<int> m_intValues;
	std::vector<float> m_floatValues;
	std::vector<double> m_doubleValues;
};


