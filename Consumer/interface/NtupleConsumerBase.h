
#pragma once

#include <boost/algorithm/string/join.hpp>

#include <TNtuple.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/DefaultValues.h"

/*
Base class for all Consumers which output some form of Ntuple
*/

template<class TTypes>
class NtupleConsumerBase: public ConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	NtupleConsumerBase() : ConsumerBase<TTypes>() {
	}

	virtual ~NtupleConsumerBase() {
	}

	void Init(setting_type const& settings, metadata_type& metadata) override {
		ConsumerBase<TTypes>::Init(settings, metadata);

		m_quantitiesVector = settings.GetQuantities();
		m_quantities = boost::algorithm::join(m_quantitiesVector, ":");

		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		
		m_ntuple = new TNtuple("ntuple",
				   ("Ntuple for Pipeline \"" + settings.GetName() + "\"").c_str(),
				   m_quantities.c_str());
	}

	std::string GetConsumerId() const override
	{
		return "ntuple";
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) override {
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings, metadata);

		// preallocated vector
		std::vector<float> array (m_quantitiesVector.size()) ;

		//iterate over string vector and fill the array for each quantity
		size_t arrayI = 0;
		for (std::vector<std::string>::iterator it = m_quantitiesVector.begin(); it != m_quantitiesVector.end(); ++it) {
			array[arrayI] = returnvalue(*it, event, product);
			++arrayI;
		}

		// add the array to the ntuple
		m_ntuple->Fill(&array[0]);
	}


	void Finish(setting_type const& settings, metadata_type const& metadata) override
	{
		RootFileHelper::SafeCd(settings.GetRootOutFile(),
								settings.GetRootFileFolder());
		
		m_ntuple->Write(m_ntuple->GetName());
	}


protected:
	TNtuple* m_ntuple;
	std::vector<std::string> m_quantitiesVector;
	std::string m_quantities;


private:
	virtual float returnvalue(std::string string, event_type const& event,
			product_type const& product ) { return DefaultValues::UndefinedFloat; };
};


