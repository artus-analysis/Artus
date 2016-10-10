
#include "Artus/KappaAnalysis/interface/KappaFactory.h"

// producer
#include "Artus/KappaAnalysis/interface/Producers/HltProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauJetProducer.h"
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
#include "Artus/KappaAnalysis/interface/Producers/GenParticleMatchingProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/MatchedLeptonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidLeptonsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GeneratorWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/LuminosityWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/NumberGeneratedEventsWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/SampleStitchingWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"
#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenPartonCounterProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/EmbeddingWeightProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/NicknameProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauMatchingProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/GenTauJetMatchingProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/GenMuonFSRProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ZProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/GenBosonProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidGenParticlesProducers.h"
#include "Artus/KappaAnalysis/interface/Producers/PFCandidatesProducer.h"

// filter
#include "Artus/KappaAnalysis/interface/Filters/RunLumiEventFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/JsonFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidObjectsFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/MinObjectsCountFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/MaxObjectsCountFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/TriggerMatchingFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/GenMatchingFilters.h"
#include "Artus/KappaAnalysis/interface/Filters/GenTauMatchingRecoParticleMinDeltaRFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidLeptonsMinDeltaRFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/GenDiLeptonDecayModeFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/GoodPrimaryVertexFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/BeamScrapingFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/HCALNoiseFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/nPUFilter.h"
#include "Artus/KappaAnalysis/interface/Filters/ZFilter.h"

// consumer
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowHistogramConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowTreeConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"
#include "Artus/KappaAnalysis/interface/Consumers/PrintHltConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/PrintEventsConsumer.h"
#include "Artus/KappaAnalysis/interface/Consumers/PrintGenParticleDecayTreeConsumer.h"
#include "Artus/Consumer/interface/RunTimeConsumer.h"
#include "Artus/Consumer/interface/RunTimeConsumer.h"



ProducerBaseUntemplated * KappaFactory::createProducer ( std::string const& id )
{
	if(id == GenTauDecayProducer().GetProducerId())
  		return new GenTauDecayProducer();
	else if(id == GenParticleProducer().GetProducerId())
  		return new GenParticleProducer();
	else if(id == GenTauJetProducer().GetProducerId())
  		return new GenTauJetProducer();
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
	else if(id == ValidGenElectronsProducer().GetProducerId())
		return new ValidGenElectronsProducer();
	else if(id == ValidGenMuonsProducer().GetProducerId())
		return new ValidGenMuonsProducer();
	else if(id == ValidGenTausProducer().GetProducerId())
		return new ValidGenTausProducer();
	else if(id == ElectronTriggerMatchingProducer().GetProducerId())
		return new ElectronTriggerMatchingProducer();
	else if(id == MuonTriggerMatchingProducer().GetProducerId())
		return new MuonTriggerMatchingProducer();
	else if(id == TauTriggerMatchingProducer().GetProducerId())
		return new TauTriggerMatchingProducer();
	else if(id == JetTriggerMatchingProducer().GetProducerId())
		return new JetTriggerMatchingProducer();
	else if(id == RecoElectronGenParticleMatchingProducer().GetProducerId())
		return new RecoElectronGenParticleMatchingProducer();
	else if(id == RecoMuonGenParticleMatchingProducer().GetProducerId())
		return new RecoMuonGenParticleMatchingProducer();
	else if(id == RecoTauGenParticleMatchingProducer().GetProducerId())
		return new RecoTauGenParticleMatchingProducer();
	else if(id == RecoJetGenParticleMatchingProducer().GetProducerId())
		return new RecoJetGenParticleMatchingProducer();
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
	else if(id == LuminosityWeightProducer().GetProducerId())
		return new LuminosityWeightProducer();
	else if(id == CrossSectionWeightProducer().GetProducerId())
		return new CrossSectionWeightProducer();
	else if(id == NumberGeneratedEventsWeightProducer().GetProducerId())
		return new NumberGeneratedEventsWeightProducer();
	else if(id == SampleStitchingWeightProducer().GetProducerId())
		return new SampleStitchingWeightProducer();
	else if(id == GenMuonFSRProducer().GetProducerId())
		return new GenMuonFSRProducer();
	// todo: uses setting not in KappaSettings
	else if(id == GeneralTmvaClassificationReader().GetProducerId())
		return new GeneralTmvaClassificationReader();
	else if(id == GenDiLeptonDecayModeProducer().GetProducerId())
		return new GenDiLeptonDecayModeProducer();
	else if(id == GenPartonCounterProducer().GetProducerId())
		return new GenPartonCounterProducer();
	else if(id == EmbeddingWeightProducer().GetProducerId())
		return new EmbeddingWeightProducer();
	else if(id == NicknameProducer().GetProducerId())
		return new NicknameProducer();
	else if(id == RecoElectronGenTauMatchingProducer().GetProducerId())
		return new RecoElectronGenTauMatchingProducer();
	else if(id == RecoMuonGenTauMatchingProducer().GetProducerId())
		return new RecoMuonGenTauMatchingProducer();
	else if(id == RecoTauGenTauMatchingProducer().GetProducerId())
		return new RecoTauGenTauMatchingProducer();
	else if(id == RecoElectronGenTauJetMatchingProducer().GetProducerId())
		return new RecoElectronGenTauJetMatchingProducer();
	else if(id == RecoMuonGenTauJetMatchingProducer().GetProducerId())
		return new RecoMuonGenTauJetMatchingProducer();
	else if(id == RecoTauGenTauJetMatchingProducer().GetProducerId())
		return new RecoTauGenTauJetMatchingProducer();
	else if(id == ZmmProducer().GetProducerId())
		return new ZmmProducer();
	else if(id == ZeeProducer().GetProducerId())
		return new ZeeProducer();
	else if(id == ZemProducer().GetProducerId())
		return new ZemProducer();
	else if(id == GenBosonFromGenParticlesProducer().GetProducerId())
		return new GenBosonFromGenParticlesProducer();
	else if(id == GenBosonDiLeptonDecayModeProducer().GetProducerId())
		return new GenBosonDiLeptonDecayModeProducer();
        else if(id == PFCandidatesProducer().GetProducerId())
                return new PFCandidatesProducer();
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
	else if(id == ValidBTaggedJetsFilter().GetFilterId())
		return new ValidBTaggedJetsFilter();
	else if(id == GenElectronsFilter().GetFilterId())
		return new GenElectronsFilter();
	else if(id == GenMuonsFilter().GetFilterId())
		return new GenMuonsFilter();
	else if(id == GenTausFilter().GetFilterId())
		return new GenTausFilter();
	else if(id == GenTauJetsFilter().GetFilterId())
		return new GenTauJetsFilter();
	else if(id == ElectronsCountFilter().GetFilterId())
		return new ElectronsCountFilter();
	else if(id == MuonsCountFilter().GetFilterId())
		return new MuonsCountFilter();
	else if(id == TausCountFilter().GetFilterId())
		return new TausCountFilter();
	else if(id == JetsCountFilter().GetFilterId())
		return new JetsCountFilter();
	else if(id == BTaggedJetsCountFilter().GetFilterId())
		return new BTaggedJetsCountFilter();
	else if(id == NonBTaggedJetsCountFilter().GetFilterId())
		return new NonBTaggedJetsCountFilter();
	else if(id == MinElectronsCountFilter().GetFilterId())
		return new MinElectronsCountFilter();
	else if(id == MinMuonsCountFilter().GetFilterId())
		return new MinMuonsCountFilter();
	else if(id == MinTausCountFilter().GetFilterId())
		return new MinTausCountFilter();
	else if(id == MinJetsCountFilter().GetFilterId())
		return new MinJetsCountFilter();
	else if(id == MinBTaggedJetsCountFilter().GetFilterId())
		return new MinBTaggedJetsCountFilter();
	else if(id == MinNonBTaggedJetsCountFilter().GetFilterId())
		return new MinNonBTaggedJetsCountFilter();
	else if(id == MaxElectronsCountFilter().GetFilterId())
		return new MaxElectronsCountFilter();
	else if(id == MaxMuonsCountFilter().GetFilterId())
		return new MaxMuonsCountFilter();
	else if(id == MaxTausCountFilter().GetFilterId())
		return new MaxTausCountFilter();
	else if(id == MaxJetsCountFilter().GetFilterId())
		return new MaxJetsCountFilter();
	else if(id == MaxBTaggedJetsCountFilter().GetFilterId())
		return new MaxBTaggedJetsCountFilter();
	else if(id == MaxNonBTaggedJetsCountFilter().GetFilterId())
		return new MaxNonBTaggedJetsCountFilter();
	else if(id == ElectronLowerPtCutsFilter().GetFilterId())
		return new ElectronLowerPtCutsFilter();
	else if(id == MuonLowerPtCutsFilter().GetFilterId())
		return new MuonLowerPtCutsFilter();
	else if(id == TauLowerPtCutsFilter().GetFilterId())
		return new TauLowerPtCutsFilter();
	else if(id == JetLowerPtCutsFilter().GetFilterId())
		return new JetLowerPtCutsFilter();
	else if(id == NonBTaggedJetLowerPtCutsFilter().GetFilterId())
		return new NonBTaggedJetLowerPtCutsFilter();
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
	else if(id == ElectronGenMatchingFilter().GetFilterId())
		return new ElectronGenMatchingFilter();
	else if(id == MuonGenMatchingFilter().GetFilterId())
		return new MuonGenMatchingFilter();
	else if(id == TauGenMatchingFilter().GetFilterId())
		return new TauGenMatchingFilter();
	else if(id == JetGenMatchingFilter().GetFilterId())
		return new JetGenMatchingFilter();
	else if(id == GenTauMatchingRecoElectronMinDeltaRFilter().GetFilterId())
		return new GenTauMatchingRecoElectronMinDeltaRFilter();
	else if(id == GenTauMatchingRecoMuonMinDeltaRFilter().GetFilterId())
		return new GenTauMatchingRecoMuonMinDeltaRFilter();
	else if(id == GenTauMatchingRecoTauMinDeltaRFilter().GetFilterId())
		return new GenTauMatchingRecoTauMinDeltaRFilter();
	else if(id == ValidElectronsMinDeltaRFilter().GetFilterId())
		return new ValidElectronsMinDeltaRFilter();
	else if(id == ValidMuonsMinDeltaRFilter().GetFilterId())
		return new ValidMuonsMinDeltaRFilter();
	else if(id == ValidTausMinDeltaRFilter().GetFilterId())
		return new ValidTausMinDeltaRFilter();
	else if(id == ValidLeptonsMinDeltaRFilter().GetFilterId())
		return new ValidLeptonsMinDeltaRFilter();
	else if(id == GenDiLeptonDecayModeFilter().GetFilterId())
		return new GenDiLeptonDecayModeFilter();
	else if(id == GoodPrimaryVertexFilter().GetFilterId())
		return new GoodPrimaryVertexFilter();
	else if(id == HCALNoiseFilter().GetFilterId())
		return new HCALNoiseFilter();
	else if(id == BeamScrapingFilter().GetFilterId())
		return new BeamScrapingFilter();
	else if(id == nPUFilter().GetFilterId())
		return new nPUFilter();
	else if(id == ZFilter().GetFilterId())
		return new ZFilter();
	else
		return FactoryBase::createFilter( id );
}

ConsumerBaseUntemplated * KappaFactory::createConsumer ( std::string const& id )
{
	if(id == KappaCutFlowHistogramConsumer().GetConsumerId())
		return new KappaCutFlowHistogramConsumer();
	if(id == KappaCutFlowTreeConsumer().GetConsumerId())
		return new KappaCutFlowTreeConsumer();
	else if(id == KappaLambdaNtupleConsumer<KappaTypes>().GetConsumerId())
		return new KappaLambdaNtupleConsumer<KappaTypes>();
	else if(id == KappaElectronsConsumer().GetConsumerId())
		return new KappaElectronsConsumer();
	else if(id == KappaMuonsConsumer().GetConsumerId())
		return new KappaMuonsConsumer();
	else if(id == KappaTausConsumer().GetConsumerId())
		return new KappaTausConsumer();
	else if(id == KappaJetsConsumer().GetConsumerId())
		return new KappaJetsConsumer();
	else if(id == KappaTaggedJetsConsumer().GetConsumerId())
		return new KappaTaggedJetsConsumer();
	else if(id == PrintHltConsumer().GetConsumerId())
		return new PrintHltConsumer();
	else if(id == PrintGenParticleDecayTreeConsumer().GetConsumerId())
		return new PrintGenParticleDecayTreeConsumer();
	else if(id == PrintEventsConsumer().GetConsumerId())
		return new PrintEventsConsumer();
	else if(id == RunTimeConsumer<KappaTypes>().GetConsumerId())
		return new RunTimeConsumer<KappaTypes>();
	else
		return FactoryBase::createConsumer( id );
}
