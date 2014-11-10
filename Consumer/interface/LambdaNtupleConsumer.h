
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
static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> CommonQuantities ;

};

template<class TTypes>
class LambdaNtupleConsumer: public ConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda;
	typedef std::function<float(EventBase const&, ProductBase const&)> float_extractor_lambda_base;

	
	static void AddQuantity ( std::string const& name, float_extractor_lambda lmbToAdd ) {
		LambdaNtupleQuantities::CommonQuantities[ name ] = [lmbToAdd] ( EventBase const& ev, ProductBase const& pd ) -> float { 
			auto const& specEv = static_cast< event_type const& > ( ev );
			auto const& specPd = static_cast< product_type const& > ( pd );
			return lmbToAdd( specEv, specPd );
		};
	}

	static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> & GetQuantities () {
		return LambdaNtupleQuantities::CommonQuantities;
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::Init(settings);
		
		// construct tree and value extractors
		m_tree = new TTree("ntuple", ("Tree for Pipeline \"" + settings.GetName() + "\"").c_str());
		
		m_values.resize(settings.GetQuantities().size());
		m_valueExtractors.clear();
		
		size_t quantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			m_tree->Branch(quantity->c_str(), &m_values[quantityIndex], (*quantity + "/F").c_str());
		
			m_valueExtractors.push_back(SafeMap::Get(LambdaNtupleQuantities::CommonQuantities, *quantity));
			
			quantityIndex++;
		}
	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings ) ARTUS_CPP11_OVERRIDE
	{
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings);
		
		size_t valueIndex = 0;
		for(typename std::vector<float_extractor_lambda_base>::iterator valueExtractor = m_valueExtractors.begin();
		    valueExtractor != m_valueExtractors.end(); ++valueExtractor)
		{
			m_values[valueIndex] = (*valueExtractor)(event, product);
			valueIndex++;
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
	
	std::vector<float_extractor_lambda_base> m_valueExtractors;
	std::vector<float> m_values;
};


