
#pragma once

#include "Artus/Consumer/interface/NtupleConsumerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"

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

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE
	{
		return "lambda_ntuple";
	}

	LambdaNtupleConsumerBase() : NtupleConsumerBase<TTypes>() {
	}

	virtual ~LambdaNtupleConsumerBase() {
	}

	virtual void Init(Pipeline<TTypes> * pset) ARTUS_CPP11_OVERRIDE {
		NtupleConsumerBase<TTypes>::Init(pset);
		
		float_extractor_lambda defaultExtractor = [](event_type const&, product_type const&) { return DefaultValues::UndefinedFloat; };
		
		// construct extractors vector
		m_valueExtractors.clear();
		m_valueExtractors.resize(this->m_quantitiesVector.size());
		transform(this->m_quantitiesVector.begin(), this->m_quantitiesVector.end(),
		        m_valueExtractors.begin(), [&](std::string quantity)
			{
				if (m_valueExtractorMap.count(quantity) > 0)
					return m_valueExtractorMap[quantity];
				else
				{
					LOG(FATAL)<<"Quantity '" << quantity << "' is not defined in the LambdaNtupleConsumer!";
					return defaultExtractor;
				}
			}
		);

	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product ) ARTUS_CPP11_OVERRIDE
	{
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
		this->m_ntuple->Fill(&array[0]);
	}


protected:
	std::map<std::string, float_extractor_lambda> m_valueExtractorMap;

private:
	std::vector<float_extractor_lambda> m_valueExtractors;

};


