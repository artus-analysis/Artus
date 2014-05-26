
#pragma once

#include <boost/algorithm/string/predicate.hpp>

#include "Artus/Consumer/interface/NtupleConsumerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <cassert>

/*
 * Fills NTuples with valueExtractors defined as lambda functions
 * This removes the string operations from its base class
 * This consumer can only be fully initilised in the constructor of an derived class
 * where the map m_valueExtractorMap is filled with analysis specific code
 */

template<class TTypes>
class LambdaNtupleConsumerBase: public NtupleConsumerBase<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<float(event_type const&, product_type const&)> float_extractor_lambda;
	LambdaNtupleConsumerBase() : NtupleConsumerBase<TTypes>() {
	}

	virtual ~LambdaNtupleConsumerBase() {
	}

	virtual void Init(Pipeline<TTypes> * pipeline) ARTUS_CPP11_OVERRIDE {
		NtupleConsumerBase<TTypes>::Init(pipeline);
		
		//float_extractor_lambda defaultExtractor = [](event_type const&, product_type const&) { return DefaultValues::UndefinedFloat; };
		
		// loop over all quantities containing "weight" (case-insensitive)
		// and try to find them in the weights map to write them out
		for (auto const & quantity : pipeline->GetSettings().GetQuantities())
		{
			if (boost::algorithm::icontains(quantity, "weight"))
			{
				if (m_valueExtractorMap.count(quantity) == 0)
				{
					LOG(INFO) << "Quantity \"" << quantity << "\" is not defined in the NtupleConsumer, therefore the values are directly taken from product.m_weights.";
					m_valueExtractorMap[quantity] = [quantity](event_type const & event, product_type const & product)
					{
						return SafeMap::GetWithDefault(product.m_weights, quantity, 1.0);
					};
				}
			}
		}
		
		// construct extractors vector
		m_valueExtractors.clear();
		for (std::vector<std::string>::iterator quantity = this->m_quantitiesVector.begin();
		     quantity != this->m_quantitiesVector.end(); ++quantity)
		{
			//m_valueExtractors.push_back(SafeMap::GetDefault(m_valueExtractorMap, *quantity, defaultExtractor));
			m_valueExtractors.push_back(SafeMap::Get(m_valueExtractorMap, *quantity));
		}
	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product ) ARTUS_CPP11_OVERRIDE
	{
		assert(m_valueExtractors.size() == this->m_quantitiesVector.size());

		// do not call NtupleConsumerBase<TTypes>::ProcessFilteredEvent due to different filling logic
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product);
		
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


protected:
	std::map<std::string, float_extractor_lambda> m_valueExtractorMap;

private:
	std::vector<float_extractor_lambda> m_valueExtractors;

};


