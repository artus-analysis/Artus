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
	typedef typename TTypes::local_product_type local_product_type;
	typedef typename TTypes::global_product_type global_product_type;
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

	static std::string GetName()
	{
		return "ntuple_consumer";
	}

	void ProcessFilteredEvent(event_type const& event,
			global_product_type const& globalProduct,
			local_product_type const& localProduct) ARTUS_CPP11_OVERRIDE {
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, globalProduct, localProduct);

		std::vector<float> array;

		//iterate over string vector and fill the array for each quantitiy
		for (stringvector::iterator it = quantities_vector.begin(); it != quantities_vector.end(); ++it)
			array.push_back(returnvalue(*it, event, globalProduct, localProduct));

		// add the array to the ntuple
		ntuple->Fill(&array[0]);
	}


	virtual void Finish()
	{
		ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}

private:
	TNtuple* ntuple;
	stringvector quantities_vector;
	std::string quantities;

	virtual float returnvalue(std::string string, event_type const& event,
			global_product_type const& globalProduct,
			local_product_type const& localProduct)
	{return 0.;}
};


