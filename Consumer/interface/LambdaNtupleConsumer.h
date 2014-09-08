
#pragma once

#include <boost/algorithm/string/predicate.hpp>

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
template<class TTypes>
class LambdaNtupleConsumer: public NtupleConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda;
	
	static std::map<std::string, float_extractor_lambda> Quantities;
	
	LambdaNtupleConsumer() : NtupleConsumerBase<TTypes>() {
	}

	virtual ~LambdaNtupleConsumer() {
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		NtupleConsumerBase<TTypes>::Init(settings);
		
		// construct extractors vector
		m_valueExtractors.clear();
		//float_extractor_lambda defaultExtractor = [](event_type const&, product_type const&) { return DefaultValues::UndefinedFloat; };
		for (std::vector<std::string>::iterator quantity = this->m_quantitiesVector.begin();
		     quantity != this->m_quantitiesVector.end(); ++quantity)
		{
			//m_valueExtractors.push_back(SafeMap::GetDefault(LambdaNtupleConsumer<TTypes>::Quantities, *quantity, defaultExtractor));
			m_valueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::Quantities, *quantity));
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
		for(typename std::vector<float_extractor_lambda>::iterator valueExtractor = m_valueExtractors.begin();
		    valueExtractor != m_valueExtractors.end(); ++valueExtractor)
		{
			array[arrayIndex] = (*valueExtractor)(event, product);
			arrayIndex++;
		}

		// add the array to the ntuple
		this->m_ntuple->Fill(&array.front());
	}


private:
	std::vector<float_extractor_lambda> m_valueExtractors;

};


template<class TTypes>
std::map<std::string, std::function<float(typename TTypes::event_type const&, typename TTypes::product_type const&)> > LambdaNtupleConsumer<TTypes>::Quantities = std::map<std::string, std::function<float(typename TTypes::event_type const&, typename TTypes::product_type const&)> >();


