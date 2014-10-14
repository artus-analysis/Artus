
#include "Artus/KappaAnalysis/interface/KappaFactory.h"

// producer
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayModeProducer.h"
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
#include "Artus/KappaAnalysis/interface/Producers/GenMatchingProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/MatchedLeptonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidLeptonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GeneratorWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/NumberGeneratedEventsWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"
#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeProducer.h"

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
#include "Artus/KappaAnalysis/interface/Filters/GenDiLeptonDecayModeFilter.h"

// consumer
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowHistogramConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"


ProducerBaseUntemplated * KappaFactory::createProducer ( std::string const& id )
{
	if(id == GenTauDecayProducer().GetProducerId())
  		return new GenTauDecayProducer();
	else if(id == GenTauDecayModeProducer().GetProducerId())
  		return new GenTauDecayModeProducer();
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
	else if(id == ValidTausProducer().GetProducerId())
		return new ValidTausProducer();
	else if(id == ValidJetsProducer().GetProducerId())
		return new ValidJetsProducer();
	else if(id == ValidTaggedJetsProducer().GetProducerId())
		return new ValidTaggedJetsProducer();
	else if(id == ValidBTaggedJetsProducer().GetProducerId())
		return new ValidBTaggedJetsProducer();
	else if(id == ElectronTriggerMatchingProducer().GetProducerId())
		return new ElectronTriggerMatchingProducer();
	else if(id == MuonTriggerMatchingProducer().GetProducerId())
		return new MuonTriggerMatchingProducer();
	else if(id == TauTriggerMatchingProducer().GetProducerId())
		return new TauTriggerMatchingProducer();
	else if(id == JetTriggerMatchingProducer().GetProducerId())
		return new JetTriggerMatchingProducer();
	else if(id == ElectronGenMatchingProducer().GetProducerId())
		return new ElectronGenMatchingProducer();
	else if(id == MuonGenMatchingProducer().GetProducerId())
		return new MuonGenMatchingProducer();
	else if(id == TauGenMatchingProducer().GetProducerId())
		return new TauGenMatchingProducer();
	else if(id == JetGenMatchingProducer().GetProducerId())
		return new JetGenMatchingProducer();
	else if(id == MatchedLeptonsProducer().GetProducerId())
		return new MatchedLeptonsProducer();
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
	else if(id == GeneralTmvaClassificationReader().GetProducerId())
		return new GeneralTmvaClassificationReader();
	else if(id == GenDiLeptonDecayModeProducer().GetProducerId())
		return new GenDiLeptonDecayModeProducer();
	else
		return FactoryBase::createProducer( id );	
}

FilterBaseUntemplated * KappaFactory::createFilter ( std::string const& id )
{
	if(id == RunLumiEventFilter().GetFilterId())
		return new RunLumiEventFilter();
	else if(id == JsonFilter().GetFilterId())
		return new JsonFilter();
	else if(id == HltFilter().GetFilterId())
		return new HltFilter();
	else if(id == ValidElectronsFilter().GetFilterId())
		return new ValidElectronsFilter();
	else if(id == ValidMuonsFilter().GetFilterId())
		return new ValidMuonsFilter();
	else if(id == ValidTausFilter().GetFilterId())
		return new ValidTausFilter();
	else if(id == ValidJetsFilter().GetFilterId())
		return new ValidJetsFilter();
	else if(id == ElectronsCountFilter().GetFilterId())
		return new ElectronsCountFilter();
	else if(id == MuonsCountFilter().GetFilterId())
		return new MuonsCountFilter();
	else if(id == TausCountFilter().GetFilterId())
		return new TausCountFilter();
	else if(id == JetsCountFilter().GetFilterId())
		return new JetsCountFilter();
	else if(id == MaxElectronsCountFilter().GetFilterId())
		return new MaxElectronsCountFilter();
	else if(id == MaxMuonsCountFilter().GetFilterId())
		return new MaxMuonsCountFilter();
	else if(id == MaxTausCountFilter().GetFilterId())
		return new MaxTausCountFilter();
	else if(id == MaxJetsCountFilter().GetFilterId())
		return new MaxJetsCountFilter();
	else if(id == ElectronLowerPtCutsFilter().GetFilterId())
		return new ElectronLowerPtCutsFilter();
	else if(id == MuonLowerPtCutsFilter().GetFilterId())
		return new MuonLowerPtCutsFilter();
	else if(id == TauLowerPtCutsFilter().GetFilterId())
		return new TauLowerPtCutsFilter();
	else if(id == JetLowerPtCutsFilter().GetFilterId())
		return new JetLowerPtCutsFilter();
	else if(id == ElectronUpperAbsEtaCutsFilter().GetFilterId())
		return new ElectronUpperAbsEtaCutsFilter();
	else if(id == MuonUpperAbsEtaCutsFilter().GetFilterId())
		return new MuonUpperAbsEtaCutsFilter();
	else if(id == TauUpperAbsEtaCutsFilter().GetFilterId())
		return new TauUpperAbsEtaCutsFilter();
	else if(id == JetUpperAbsEtaCutsFilter().GetFilterId())
		return new JetUpperAbsEtaCutsFilter();
	else if(id == ElectronTriggerMatchingFilter().GetFilterId())
		return new ElectronTriggerMatchingFilter();
	else if(id == MuonTriggerMatchingFilter().GetFilterId())
		return new MuonTriggerMatchingFilter();
	else if(id == TauTriggerMatchingFilter().GetFilterId())
		return new TauTriggerMatchingFilter();
	else if(id == JetTriggerMatchingFilter().GetFilterId())
		return new JetTriggerMatchingFilter();
	else if(id == GenDiLeptonDecayModeFilter().GetFilterId())
		return new GenDiLeptonDecayModeFilter();
	else
		return FactoryBase::createFilter( id );
}

ConsumerBaseUntemplated * KappaFactory::createConsumer ( std::string const& id )
{
	if(id == KappaCutFlowHistogramConsumer().GetConsumerId())
		return new KappaCutFlowHistogramConsumer();
	else if(id == KappaLambdaNtupleConsumer<KappaTypes>().GetConsumerId())
		return new KappaLambdaNtupleConsumer<KappaTypes>();
	else
		return FactoryBase::createConsumer( id );
}