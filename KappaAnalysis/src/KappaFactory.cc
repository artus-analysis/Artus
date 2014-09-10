
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
	else if(id == GeneralTmvaClassificationReader<KappaTypes>().GetProducerId())
		return new GeneralTmvaClassificationReader<KappaTypes>();
	else
		return FactoryBase/*<KappaTypes>*/::createProducer( id );	
}

FilterBaseUntemplated * KappaFactory::createFilter ( std::string const& id )
{
	if(id == RunLumiEventFilter<KappaTypes>().GetFilterId())
		return new RunLumiEventFilter<KappaTypes>();
	else if(id == JsonFilter<KappaTypes>().GetFilterId())
		return new JsonFilter<KappaTypes>();
	else if(id == HltFilter<KappaTypes>().GetFilterId())
		return new HltFilter<KappaTypes>();
	else if(id == ValidElectronsFilter<KappaTypes>().GetFilterId())
		return new ValidElectronsFilter<KappaTypes>();
	else if(id == ValidMuonsFilter<KappaTypes>().GetFilterId())
		return new ValidMuonsFilter<KappaTypes>();
	else if(id == ValidTausFilter<KappaTypes>().GetFilterId())
		return new ValidTausFilter<KappaTypes>();
	else if(id == ValidJetsFilter<KappaTypes>().GetFilterId())
		return new ValidJetsFilter<KappaTypes>();
	else if(id == ElectronsCountFilter<KappaTypes>().GetFilterId())
		return new ElectronsCountFilter<KappaTypes>();
	else if(id == MuonsCountFilter<KappaTypes>().GetFilterId())
		return new MuonsCountFilter<KappaTypes>();
	else if(id == TausCountFilter<KappaTypes>().GetFilterId())
		return new TausCountFilter<KappaTypes>();
	else if(id == JetsCountFilter<KappaTypes>().GetFilterId())
		return new JetsCountFilter<KappaTypes>();
	else if(id == MaxElectronsCountFilter<KappaTypes>().GetFilterId())
		return new MaxElectronsCountFilter<KappaTypes>();
	else if(id == MaxMuonsCountFilter<KappaTypes>().GetFilterId())
		return new MaxMuonsCountFilter<KappaTypes>();
	else if(id == MaxTausCountFilter<KappaTypes>().GetFilterId())
		return new MaxTausCountFilter<KappaTypes>();
	else if(id == MaxJetsCountFilter<KappaTypes>().GetFilterId())
		return new MaxJetsCountFilter<KappaTypes>();
	else if(id == ElectronLowerPtCutsFilter<KappaTypes>().GetFilterId())
		return new ElectronLowerPtCutsFilter<KappaTypes>();
	else if(id == MuonLowerPtCutsFilter<KappaTypes>().GetFilterId())
		return new MuonLowerPtCutsFilter<KappaTypes>();
	else if(id == TauLowerPtCutsFilter<KappaTypes>().GetFilterId())
		return new TauLowerPtCutsFilter<KappaTypes>();
	else if(id == JetLowerPtCutsFilter<KappaTypes>().GetFilterId())
		return new JetLowerPtCutsFilter<KappaTypes>();
	else if(id == ElectronUpperAbsEtaCutsFilter<KappaTypes>().GetFilterId())
		return new ElectronUpperAbsEtaCutsFilter<KappaTypes>();
	else if(id == MuonUpperAbsEtaCutsFilter<KappaTypes>().GetFilterId())
		return new MuonUpperAbsEtaCutsFilter<KappaTypes>();
	else if(id == TauUpperAbsEtaCutsFilter<KappaTypes>().GetFilterId())
		return new TauUpperAbsEtaCutsFilter<KappaTypes>();
	else if(id == JetUpperAbsEtaCutsFilter<KappaTypes>().GetFilterId())
		return new JetUpperAbsEtaCutsFilter<KappaTypes>();
	else if(id == ElectronTriggerMatchingFilter<KappaTypes>().GetFilterId())
		return new ElectronTriggerMatchingFilter<KappaTypes>();
	else if(id == MuonTriggerMatchingFilter<KappaTypes>().GetFilterId())
		return new MuonTriggerMatchingFilter<KappaTypes>();
	else if(id == TauTriggerMatchingFilter<KappaTypes>().GetFilterId())
		return new TauTriggerMatchingFilter<KappaTypes>();
	else if(id == JetTriggerMatchingFilter<KappaTypes>().GetFilterId())
		return new JetTriggerMatchingFilter<KappaTypes>();
	else
		return FactoryBase::createFilter( id );
}

ConsumerBaseUntemplated * KappaFactory::createConsumer ( std::string const& id )
{
	if(id == KappaCutFlowHistogramConsumer<KappaTypes>().GetConsumerId())
		return new KappaCutFlowHistogramConsumer<KappaTypes>();
	else if(id == KappaLambdaNtupleConsumer<KappaTypes>().GetConsumerId())
		return new KappaLambdaNtupleConsumer<KappaTypes>();
	else
		return FactoryBase::createConsumer( id );
}
