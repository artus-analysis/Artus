
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/Pipeline.h"

#include "Filters/HltFilter.h"


template<class TTypes>
class KappaPipelineInitializer: public PipelineInitilizerBase<HttTypes> {
public:

	virtual void InitPipeline(HttPipeline * pLine,  HttPipelineSettings const& pset) const ARTUS_CPP11_OVERRIDE {

		BOOST_FOREACH(std::string producerId, pset.GetLocalProducers())
		{
			// TODO
		}

		BOOST_FOREACH(std::string filterId, pset.GetFilters())
		{
			if(filterId == HltFilter<TTypes>().GetFilterId()) {
				pLine->AddFilter(new HltFilter<TTypes>());
			}
		}

		BOOST_FOREACH(std::string consumerId, pset.GetConsumers())
		{
			// TODO
		}

	}
};
