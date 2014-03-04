
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/Pipeline.h"


template<class TTypes>
class KappaPipelineInitializer: public PipelineInitilizerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef Pipeline<TTypes> pipeline_type;

	virtual void InitPipeline(pipeline_type * pLine, setting_type const& pset) const ARTUS_CPP11_OVERRIDE
	{
		// new implementation of pure PipelineInitilizerBase::InitPipeline
	}
};
