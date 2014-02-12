/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <TNtuple.h>

#include "Artus/Core/interface/ConsumerBase.h"

/*
Base class for all Consumers which output some form of Ntuple
*/

template<class TTypes>
class NtupleConsumerBase: public ConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	NtupleConsumerBase() : ConsumerBase<TTypes>() {
	}

	virtual ~NtupleConsumerBase() {
	}

	virtual void Init(Pipeline<TTypes> * pset) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::Init(pset);

		quantities_vector = pset->GetSettings().GetQuantities();
		quantities = boost::algorithm::join(quantities_vector, ":");

		ntuple = new TNtuple(this->GetPipelineSettings().GetName().c_str(),
							 this->GetPipelineSettings().GetName().c_str(),
							 quantities.c_str());
	}

	virtual std::string GetConsumerId()
	{
		return "ntuple";
	}

	virtual void ProcessFilteredEvent(event_type const& event,
			product_type const& product ) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product);

		// preallocated vector
		std::vector<float> array (quantities_vector.size()) ;

		//iterate over string vector and fill the array for each quantity
		size_t arrayI = 0;
		for (stringvector::iterator it = quantities_vector.begin(); it != quantities_vector.end(); ++it) {
			array[ arrayI ] = returnvalue(*it, event, product);
			arrayI ++;
		}

		// add the array to the ntuple
		ntuple->Fill(&array[0]);
	}


	virtual void Finish() ARTUS_CPP11_OVERRIDE
	{
		ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}


protected:
	TNtuple* ntuple;
	stringvector quantities_vector;
	std::string quantities;


private:
	virtual float returnvalue(std::string string, event_type const& event,
			product_type const& product )
	{return 0.;}
};


