
#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/FactoryBase.h"

// producer
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidElectronsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidMuonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidTausProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"

// filter
#include "Artus/KappaAnalysis/interface/Filters/JsonFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidObjectsFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"

// consumer


template<class TTypes>
class KappaFactory: public FactoryBase<TTypes> {
public:

	KappaFactory() : FactoryBase<TTypes>() {
	}

	virtual ~KappaFactory() {}

	virtual ProducerBase<TTypes> * createProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if(id == GenTauDecayProducer<TTypes>().GetProducerId())
	  		return new GenTauDecayProducer<TTypes>();
		else if(id == HltProducer<TTypes>().GetProducerId())
			return new HltProducer<TTypes>();
		else if(id == ValidElectronsProducer<TTypes>().GetProducerId())
			return new ValidElectronsProducer<TTypes>();
		else if(id == ValidMuonsProducer<TTypes>().GetProducerId())
			return new ValidMuonsProducer<TTypes>();
		else if(id == ValidTausProducer<TTypes>().GetProducerId())
			return new ValidTausProducer<TTypes>();
		else if(id == ValidJetsProducer<TTypes>().GetProducerId())
			return new ValidJetsProducer<TTypes>();
		else if(id == ValidTaggedJetsProducer<TTypes>().GetProducerId())
			return new ValidTaggedJetsProducer<TTypes>();
		else if(id == PUWeightProducer<TTypes>().GetProducerId())
			return new PUWeightProducer<TTypes>();
		else if(id == EventWeightProducer<TTypes>().GetProducerId())
			return new EventWeightProducer<TTypes>();
		else
			return FactoryBase<TTypes>::createProducer( id );	
	}

	virtual FilterBase<TTypes> * createFilter ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if(id == JsonFilter<TTypes>().GetFilterId())
			return new JsonFilter<TTypes>();
		else if(id == HltFilter<TTypes>().GetFilterId())
			return new HltFilter<TTypes>();
		else if(id == ValidElectronsFilter<TTypes>().GetFilterId())
			return new ValidElectronsFilter<TTypes>();
		else if(id == ValidMuonsFilter<TTypes>().GetFilterId())
			return new ValidMuonsFilter<TTypes>();
		else if(id == ValidTausFilter<TTypes>().GetFilterId())
			return new ValidTausFilter<TTypes>();
		else if(id == ValidJetsFilter<TTypes>().GetFilterId())
			return new ValidJetsFilter<TTypes>();
		else if(id == ElectronsCountFilter<TTypes>().GetFilterId())
			return new ElectronsCountFilter<TTypes>();
		else if(id == MuonsCountFilter<TTypes>().GetFilterId())
			return new MuonsCountFilter<TTypes>();
		else if(id == TausCountFilter<TTypes>().GetFilterId())
			return new TausCountFilter<TTypes>();
		else if(id == JetsCountFilter<TTypes>().GetFilterId())
			return new JetsCountFilter<TTypes>();
		else
			return FactoryBase<TTypes>::createFilter( id );
	}

	virtual ConsumerBase<TTypes> * createConsumer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		return FactoryBase<TTypes>::createConsumer( id );
	}

};
