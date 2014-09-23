
#pragma once

#include <boost/algorithm/string/predicate.hpp>

#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Consumer/interface/NtupleConsumerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <cassert>

/**
 * Fills NTuples with valueExtractors defined as lambda functions
 * This removes the string operations from its base class
 * This consumer can only be fully initilised in the constructor of an derived class
 * where the map LambdaNtupleConsumer<TTypes>::Quantities is filled with analysis specific code
 */


class LambdaNtupleQuantities {
public:
static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> CommonQuantities ;

};

template<class TTypes>
class LambdaNtupleConsumer: public NtupleConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda;
	typedef std::function<float(EventBase const&, ProductBase const&)> float_extractor_lambda_base;
	
	LambdaNtupleConsumer() : NtupleConsumerBase<TTypes>() {
	}

	virtual ~LambdaNtupleConsumer() {
	}

	
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
		NtupleConsumerBase<TTypes>::Init(settings);
		
		// construct extractors vector
		m_valueExtractors.clear();

		for (std::vector<std::string>::iterator quantity = this->m_quantitiesVector.begin();
		     quantity != this->m_quantitiesVector.end(); ++quantity)
		{
			m_valueExtractors.push_back(SafeMap::Get(LambdaNtupleQuantities::CommonQuantities, *quantity));
		}
	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings ) ARTUS_CPP11_OVERRIDE
	{
		assert(m_valueExtractors.size() == this->m_quantitiesVector.size());

		// do not call NtupleConsumerBase<TTypes>::ProcessFilteredEvent due to different filling logic
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings);
		
		// preallocated vector
		std::vector<float> array (this->m_quantitiesVector.size());
		
		size_t arrayIndex = 0;
		for(typename std::vector<float_extractor_lambda_base>::iterator valueExtractor = m_valueExtractors.begin();
		    valueExtractor != m_valueExtractors.end(); ++valueExtractor)
		{
			array[arrayIndex] = (*valueExtractor)(event, product);
			arrayIndex++;
		}

		// add the array to the ntuple
		this->m_ntuple->Fill(&array.front());
	}


private:
	std::vector<float_extractor_lambda_base> m_valueExtractors;

};


