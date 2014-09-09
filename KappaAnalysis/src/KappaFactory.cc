
#include "Artus/KappaAnalysis/interface/KappaFactory.h"

// producer
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ElectronCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/MuonCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/TauCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/JetCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidElectronsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidMuonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidTausProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidBTaggedJetsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/TriggerMatchingProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidLeptonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GeneratorWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/NumberGeneratedEventsWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"

// filter
#include "Artus/KappaAnalysis/interface/Filters/RunLumiEventFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/JsonFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidObjectsFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/MaxObjectsCountFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/TriggerMatchingFilters.h"

// consumer
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowHistogramConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"


ProducerBaseUntemplated * KappaFactory::createProducer ( std::string const& id )
{
	if(id == GenTauDecayProducer().GetProducerId())
  		return new GenTauDecayProducer();
	else if(id == GenParticleProducer().GetProducerId())
  		return new GenParticleProducer();
	else if(id == HltProducer().GetProducerId())
		return new HltProducer();
	else if(id == ElectronCorrectionsProducer().GetProducerId())
		return new ElectronCorrectionsProducer();
	else if(id == MuonCorrectionsProducer().GetProducerId())
		return new MuonCorrectionsProducer();
	else if(id == TauCorrectionsProducer().GetProducerId())
		return new TauCorrectionsProducer();
	else if(id == JetCorrectionsProducer().GetProducerId())
		return new JetCorrectionsProducer();
	else if(id == TaggedJetCorrectionsProducer().GetProducerId())
		return new TaggedJetCorrectionsProducer();
	else if(id == ValidElectronsProducer<KappaTypes>().GetProducerId())
		return new ValidElectronsProducer<KappaTypes>();
	else if(id == ValidMuonsProducer<KappaTypes>().GetProducerId())
		return new ValidMuonsProducer<KappaTypes>();
	else if(id == ValidTausProducer<KappaTypes>().GetProducerId())
		return new ValidTausProducer<KappaTypes>();
	else if(id == ValidJetsProducer<KappaTypes>().GetProducerId())
		return new ValidJetsProducer<KappaTypes>();
	else if(id == ValidTaggedJetsProducer<KappaTypes>().GetProducerId())
		return new ValidTaggedJetsProducer<KappaTypes>();
	else if(id == ValidBTaggedJetsProducer<KappaTypes>().GetProducerId())
		return new ValidBTaggedJetsProducer<KappaTypes>();
	else if(id == ElectronTriggerMatchingProducer<KappaTypes>().GetProducerId())
		return new ElectronTriggerMatchingProducer<KappaTypes>();
	else if(id == MuonTriggerMatchingProducer<KappaTypes>().GetProducerId())
		return new MuonTriggerMatchingProducer<KappaTypes>();
	else if(id == TauTriggerMatchingProducer<KappaTypes>().GetProducerId())
		return new TauTriggerMatchingProducer<KappaTypes>();
	else if(id == JetTriggerMatchingProducer<KappaTypes>().GetProducerId())
		return new JetTriggerMatchingProducer<KappaTypes>();
	else if(id == ValidLeptonsProducer().GetProducerId())
		return new ValidLeptonsProducer();
	else if(id == PUWeightProducer().GetProducerId())
		return new PUWeightProducer();
	else if(id == EventWeightProducer().GetProducerId())
		return new EventWeightProducer();
	else if(id == GeneratorWeightProducer().GetProducerId())
		return new GeneratorWeightProducer();
	else if(id == CrossSectionWeightProducer().GetProducerId())
		return new CrossSectionWeightProducer();
	else if(id == NumberGeneratedEventsWeightProducer().GetProducerId())
		return new NumberGeneratedEventsWeightProducer();
	// todo: uses setting not in KappaSettings
	/*else if(id == GeneralTmvaClassificationReader().GetProducerId())
		return new GeneralTmvaClassificationReader();*/
	else
		return FactoryBase/*<TTypes>*/::createProducer( id );	
}

FilterBaseUntemplated * KappaFactory::createFilter ( std::string const& id )
{
	/*if(id == RunLumiEventFilter<TTypes>().GetFilterId())
		return new RunLumiEventFilter<TTypes>();
	else if(id == JsonFilter<TTypes>().GetFilterId())
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
	else if(id == MaxElectronsCountFilter<TTypes>().GetFilterId())
		return new MaxElectronsCountFilter<TTypes>();
	else if(id == MaxMuonsCountFilter<TTypes>().GetFilterId())
		return new MaxMuonsCountFilter<TTypes>();
	else if(id == MaxTausCountFilter<TTypes>().GetFilterId())
		return new MaxTausCountFilter<TTypes>();
	else if(id == MaxJetsCountFilter<TTypes>().GetFilterId())
		return new MaxJetsCountFilter<TTypes>();
	else if(id == ElectronLowerPtCutsFilter<TTypes>().GetFilterId())
		return new ElectronLowerPtCutsFilter<TTypes>();
	else if(id == MuonLowerPtCutsFilter<TTypes>().GetFilterId())
		return new MuonLowerPtCutsFilter<TTypes>();
	else if(id == TauLowerPtCutsFilter<TTypes>().GetFilterId())
		return new TauLowerPtCutsFilter<TTypes>();
	else if(id == JetLowerPtCutsFilter<TTypes>().GetFilterId())
		return new JetLowerPtCutsFilter<TTypes>();
	else if(id == ElectronUpperAbsEtaCutsFilter<TTypes>().GetFilterId())
		return new ElectronUpperAbsEtaCutsFilter<TTypes>();
	else if(id == MuonUpperAbsEtaCutsFilter<TTypes>().GetFilterId())
		return new MuonUpperAbsEtaCutsFilter<TTypes>();
	else if(id == TauUpperAbsEtaCutsFilter<TTypes>().GetFilterId())
		return new TauUpperAbsEtaCutsFilter<TTypes>();
	else if(id == JetUpperAbsEtaCutsFilter<TTypes>().GetFilterId())
		return new JetUpperAbsEtaCutsFilter<TTypes>();
	else if(id == ElectronTriggerMatchingFilter<TTypes>().GetFilterId())
		return new ElectronTriggerMatchingFilter<TTypes>();
	else if(id == MuonTriggerMatchingFilter<TTypes>().GetFilterId())
		return new MuonTriggerMatchingFilter<TTypes>();
	else if(id == TauTriggerMatchingFilter<TTypes>().GetFilterId())
		return new TauTriggerMatchingFilter<TTypes>();
	else if(id == JetTriggerMatchingFilter<TTypes>().GetFilterId())
		return new JetTriggerMatchingFilter<TTypes>();
	else*/
		return FactoryBase/*<TTypes>*/::createFilter( id );
}

ConsumerBaseUntemplated * KappaFactory::createConsumer ( std::string const& id )
{
	/*if(id == KappaCutFlowHistogramConsumer<TTypes>().GetConsumerId())
		return new KappaCutFlowHistogramConsumer<TTypes>();
	else if(id == KappaLambdaNtupleConsumer<TTypes>().GetConsumerId())
		return new KappaLambdaNtupleConsumer<TTypes>();
	else*/
		return FactoryBase/*<TTypes>*/::createConsumer( id );
}
