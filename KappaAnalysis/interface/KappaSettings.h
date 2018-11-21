#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/DefaultValues.h"

/**
   \brief Reads settings for all parts of the KappaAnalysis code from a prepared
   json configuration file.

   Defines settings that can be obtained from the json configuration file.
*/

class KappaSettings: public SettingsBase {
public:

	IMPL_SETTING_DEFAULT(bool, BatchMode, false);

	IMPL_SETTING(std::string, Nickname);

	/// name of electron collection in kappa tupl
	IMPL_SETTING_DEFAULT(std::string, Electrons, "");
	IMPL_SETTING_DEFAULT(std::string, ElectronMetadata, "");

	/// name of muon collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Muons, "");

	/// name of tau collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Taus, "");
	IMPL_SETTING_DEFAULT(std::string, L1Taus, "");
	IMPL_SETTING_DEFAULT(std::string, TauMetadata, "");
	IMPL_SETTING_DEFAULT(std::string, GenTaus, "");
	IMPL_SETTING_DEFAULT(std::string, GenTauJets, "");

	/// name of jet collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, BasicJets, "");
	IMPL_SETTING_DEFAULT(std::string, GenJets, "");
	IMPL_SETTING_DEFAULT(bool, UseKLVGenJets, false);

	/// name of tagged jet collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TaggedJets, "");

	/// name of pileup density branch in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, PileupDensity, "");

	/// name of MET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Met, "");

	/// name of PUPPI MET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, PuppiMet, "");

	/// name of GenMET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenMet, "");

	/// names of PF candidate collections in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, PFChargedHadronsPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFChargedHadronsNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFNeutralHadronsNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFPhotonsNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFAllChargedParticlesNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFAllChargedParticlesPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PackedPFCandidates, "");

	/// name of beamspot collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, BeamSpot, "");

	/// name of vertexSummary collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, VertexSummary, "");

	/// name of refit vertices collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, RefitVertices, "");
	IMPL_SETTING_DEFAULT(std::string, RefitBSVertices, "");

	/// name of track summary collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TrackSummary, "");

	/// name of HCALNoise summary collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, HCALNoiseSummary, "");

	/// name of trigger infos and objects collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TriggerInfos, "");
	IMPL_SETTING_DEFAULT(std::string, TriggerObjects, "");

	/// name of genParticle collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenParticles, "");
	IMPL_SETTING_DEFAULT(std::string, LheParticles, "");

	/// name of eventInfo in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, EventInfo, "");
	IMPL_SETTING_DEFAULT(std::string, GenEventInfoMetadata, "");

	/// name of lumiInfo in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, LumiInfo, "");

	/// name of runInfo in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, RunInfo, "");

	/// name of filterMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterMetadata, "");

	/// name of filterSummary in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterSummary, "");

	/// name of TaggerMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, JetMetadata, "");


	IMPL_SETTING(float, CrossSection);
	// Events will be weighted with the inverse of int. luminosity
	IMPL_SETTING(float, IntLuminosity);
	IMPL_SETTING(int, NumberGeneratedEvents);

	IMPL_SETTING_STRINGLIST_DEFAULT(JsonFiles, {});
	IMPL_SETTING_DEFAULT(int, PassRunLow, 0);
	IMPL_SETTING_DEFAULT(int, PassRunHigh, 0);

	// Good Primary Vertex Filter
	IMPL_SETTING(float, MaxPrimaryVertexZ);
	IMPL_SETTING(float, MaxPrimaryVertexRho);
	IMPL_SETTING(float, MinPrimaryVertexFitnDOF);

	// Beam Scraping Filter
	IMPL_SETTING(double, MinPurityRatio);

	IMPL_SETTING_UINT64LIST_DEFAULT(RunWhitelist, {});
	IMPL_SETTING_UINT64LIST_DEFAULT(RunBlacklist, {});
	IMPL_SETTING_UINT64LIST_DEFAULT(LumiWhitelist, {});
	IMPL_SETTING_UINT64LIST_DEFAULT(LumiBlacklist, {});
	IMPL_SETTING_UINT64LIST_DEFAULT(EventWhitelist, {});
	IMPL_SETTING_UINT64LIST_DEFAULT(EventBlacklist, {});
	IMPL_SETTING_DEFAULT(bool, MatchRunLumiEventTuples, false);

	IMPL_SETTING_STRINGLIST_DEFAULT(HltPaths, {});
	IMPL_SETTING_DEFAULT(bool, AllowPrescaledTrigger, true);

	IMPL_SETTING_DEFAULT(bool, NoHltFiltering, false);

	IMPL_SETTING_STRINGLIST_DEFAULT(TauTriggerCheckL1Match, {});

	IMPL_SETTING_STRINGLIST_DEFAULT(ElectronTriggerFilterNames, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(MuonTriggerFilterNames, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(TauTriggerFilterNames, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(JetTriggerFilterNames, {});

	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingElectrons, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingMuons, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingTaus, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingJets, 0.5f);

	IMPL_SETTING_DEFAULT(float, TriggerObjectLowerPtCut, -1.0);

	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingElectrons, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingMuons, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingTaus, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingJets, true);

	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoElectronGenParticle, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoMuonGenParticle, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoTauGenParticle, 0.5f);

	IMPL_SETTING_DEFAULT(float, MinDeltaRMatchedRecoElectrons, 0.5f);
	IMPL_SETTING_DEFAULT(float, MinDeltaRMatchedRecoMuons, 0.5f);
	IMPL_SETTING_DEFAULT(float, MinDeltaRMatchedRecoTaus, 0.5f);

	IMPL_SETTING_DEFAULT(float, MinDeltaRValidElectrons, 0.5f);
	IMPL_SETTING_DEFAULT(float, MinDeltaRValidMuons, 0.5f);
	IMPL_SETTING_DEFAULT(float, MinDeltaRValidTaus, 0.5f);
	IMPL_SETTING_DEFAULT(float, MinDeltaRValidLeptons, 0.5f);

	std::vector<int> RecoElectronMatchingGenParticlePdgIds = {-11, 11};
	std::vector<int> RecoMuonMatchingGenParticlePdgIds = {-13, 13};
	std::vector<int> RecoTauMatchingGenParticlePdgIds = {-15, 15};
	IMPL_SETTING_INTLIST_DEFAULT(RecoElectronMatchingGenParticlePdgIds, RecoElectronMatchingGenParticlePdgIds);
	IMPL_SETTING_INTLIST_DEFAULT(RecoMuonMatchingGenParticlePdgIds, RecoMuonMatchingGenParticlePdgIds);
	IMPL_SETTING_INTLIST_DEFAULT(RecoTauMatchingGenParticlePdgIds, RecoTauMatchingGenParticlePdgIds);

	IMPL_SETTING_DEFAULT(int, RecoElectronMatchingGenParticleStatus, -1);
	IMPL_SETTING_DEFAULT(int, RecoMuonMatchingGenParticleStatus, -1);
	IMPL_SETTING_DEFAULT(int, RecoTauMatchingGenParticleStatus, -1);

	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoJetGenParticle, 0.3f);
	IMPL_SETTING_DEFAULT(std::string, JetMatchingAlgorithm, "algorithmic");
	IMPL_SETTING_DEFAULT(int, RecoJetMatchingGenParticleStatus, 3);  //keep pythia6 status as default for back-compatibility

	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenParticleMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenParticleMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenParticleMatchingRecoTaus, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenParticleMatchingRecoJets, false);

	IMPL_SETTING_DEFAULT(bool, InvalidateGenParticleMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenParticleMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenParticleMatchingRecoTaus, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenParticleMatchingRecoJets, false);

	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoElectronGenTau, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoMuonGenTau, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoTauGenTau, 0.5f);

	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauMatchingRecoTaus, false);

	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauMatchingRecoTaus, false);

	IMPL_SETTING_DEFAULT(bool, RecoElectronMatchingGenParticleMatchAllElectrons, false);
	IMPL_SETTING_DEFAULT(bool, RecoMuonMatchingGenParticleMatchAllMuons, false);
	IMPL_SETTING_DEFAULT(bool, RecoTauMatchingGenParticleMatchAllTaus, false);

	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoElectronGenTauJet, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoMuonGenTauJet, 0.5f);
	IMPL_SETTING_DEFAULT(float, DeltaRMatchingRecoTauGenTauJet, 0.5f);

	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauJetMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauJetMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonGenTauJetMatchingRecoTaus, false);

	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauJetMatchingRecoElectrons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauJetMatchingRecoMuons, false);
	IMPL_SETTING_DEFAULT(bool, InvalidateGenTauJetMatchingRecoTaus, false);

	IMPL_SETTING_DEFAULT(bool, MatchAllElectronsGenTau, false);
	IMPL_SETTING_DEFAULT(bool, MatchAllMuonsGenTau, false);
	IMPL_SETTING_DEFAULT(bool, MatchAllTausGenTau, false);
	IMPL_SETTING_DEFAULT(bool, MatchGenTauDecayMode, false);
	IMPL_SETTING_DEFAULT(bool, UseUWGenMatching, false);

	IMPL_SETTING(int, Year);

	IMPL_SETTING_DEFAULT(bool, DirectIso, true);

	IMPL_SETTING_DEFAULT(std::string, ValidMuonsInput, "auto");
	IMPL_SETTING(std::string, MuonID);
	IMPL_SETTING(std::string, MuonIsoType);
	IMPL_SETTING(std::string, MuonIso);

	IMPL_SETTING_DEFAULT(std::string, ValidElectronsInput, "auto");
	IMPL_SETTING(std::string, ElectronID);
	IMPL_SETTING(std::string, ElectronIsoType);
	IMPL_SETTING(std::string, ElectronIso);
	IMPL_SETTING(std::string, ElectronReco);

	IMPL_SETTING_DEFAULT(std::string, ValidTausInput, "auto");
	IMPL_SETTING_STRINGLIST_DEFAULT(TauDiscriminators, {});
	IMPL_SETTING_DEFAULT(std::string, TauID, "none");
	IMPL_SETTING_DEFAULT(bool, TauUseOldDMs, false);

	IMPL_SETTING_STRINGLIST_DEFAULT(JetEnergyCorrectionParameters, {});
	IMPL_SETTING_DEFAULT(std::string, JetEnergyCorrectionUncertaintyParameters, "");
	IMPL_SETTING_DEFAULT(std::string, JetEnergyCorrectionUncertaintySource, "");
	IMPL_SETTING_DEFAULT(float, JetEnergyCorrectionUncertaintyShift, 0.0f);

	IMPL_SETTING_DEFAULT(std::string, ValidJetsInput, "auto");
	IMPL_SETTING(std::string, JetID);
	IMPL_SETTING_DEFAULT(float, JetLeptonLowerDeltaRCut, -1.0f);
	IMPL_SETTING_DEFAULT(std::string, JetIDVersion, "2010");
	IMPL_SETTING_DEFAULT(std::string, PuJetIDFullDiscrName, "puJetIDFullDiscriminant");
	IMPL_SETTING_STRINGLIST_DEFAULT(PuJetIDs, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(JetTaggerLowerCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(JetTaggerUpperCuts, {});

	IMPL_SETTING(int, NElectrons);
	IMPL_SETTING(int, NMuons);
	IMPL_SETTING(int, NTaus);
	IMPL_SETTING(int, NJets);
	IMPL_SETTING(int, NBTaggedJets);
	IMPL_SETTING(int, NNonBTaggedJets);

	IMPL_SETTING(int, MinNElectrons);
	IMPL_SETTING(int, MinNMuons);
	IMPL_SETTING(int, MinNTaus);
	IMPL_SETTING(int, MinNJets);
	IMPL_SETTING(int, MinNBTaggedJets);
	IMPL_SETTING(int, MinNNonBTaggedJets);

	IMPL_SETTING(int, MaxNElectrons);
	IMPL_SETTING(int, MaxNMuons);
	IMPL_SETTING(int, MaxNTaus);
	IMPL_SETTING(int, MaxNJets);
	IMPL_SETTING(int, MaxNBTaggedJets);
	IMPL_SETTING(int, MaxNNonBTaggedJets);

	//settings for nPUFilter
	IMPL_SETTING(int, MinNPU);
	IMPL_SETTING(int, MaxNPU);

	IMPL_SETTING_DEFAULT(size_t, MinNMatchedElectrons, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedMuons, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedTaus, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedJets, 0);

	IMPL_SETTING_STRINGLIST_DEFAULT(ElectronLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(MuonLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(TauLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(JetLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenJetLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(NonBTaggedJetLowerPtCuts, {});

	IMPL_SETTING_STRINGLIST_DEFAULT(ElectronUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(MuonUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(TauUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(JetUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenJetUpperAbsEtaCuts, {});

	IMPL_SETTING(std::string, BTagger);
	IMPL_SETTING(std::string, BTaggedJetDdeepCSVName);
	IMPL_SETTING_DEFAULT(float, BTaggedJetAbsEtaCut, 0.0f);
	IMPL_SETTING_DEFAULT(std::string, BTaggedJetTrackCountingHighEffName, "TrackCountingHighEffBJetTags");
	IMPL_SETTING_DEFAULT(std::string, BTaggedJetCombinedSecondaryVertexName, "CombinedSecondaryVertexBJetTags");
	IMPL_SETTING_STRINGLIST_DEFAULT(BTagWPs, {"medium"});
	IMPL_SETTING_STRINGLIST_DEFAULT(BTaggerWorkingPoints, {});
	IMPL_SETTING_DEFAULT(bool, ApplyBTagSF, false);
	IMPL_SETTING(std::string, BTagSFMethod);
	IMPL_SETTING_DEFAULT(float, BTagShift, 0.0f);
	IMPL_SETTING_DEFAULT(float, BMistagShift, 0.0f);

	// Needed by the GenBosonProducers
	IMPL_SETTING_INTLIST_DEFAULT(BosonPdgIds, {23});
	IMPL_SETTING_INTLIST_DEFAULT(BosonStatuses, {3});  //keep pythia6 status as default for back-compatibility

	// Needed by the ValidGenParticleProducers
	IMPL_SETTING_STRINGLIST_DEFAULT(GenElectronLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenMuonLowerPtCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenTauLowerPtCuts, {});

	IMPL_SETTING_STRINGLIST_DEFAULT(GenElectronUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenMuonUpperAbsEtaCuts, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(GenTauUpperAbsEtaCuts, {});

	/// Needed by the GenPartonCounterProducer
	IMPL_SETTING_DEFAULT(int, PartonStatus, 3);  //keep pythia6 status as default for back-compatibility

	/// Needed by the GenDiLeptonDecayModeFilter
	IMPL_SETTING(std::string, GenDiLeptonDecayMode);
	IMPL_SETTING_DEFAULT(bool, GenDiLeptonDecayModeFilterLoose, false);
	IMPL_SETTING_DEFAULT(bool, InvertGenDiLeptonDecayMode, false);

	IMPL_SETTING_STRINGLIST_DEFAULT(GenParticleTypes, {});
	IMPL_SETTING_INTLIST_DEFAULT(GenParticlePdgIds, {});
	IMPL_SETTING_DEFAULT(int, GenParticleStatus, -1);
	IMPL_SETTING_DEFAULT(int, GenElectronStatus, -1);
	IMPL_SETTING_DEFAULT(int, GenMuonStatus, -1);
	IMPL_SETTING_DEFAULT(int, GenTauStatus, -1);
	IMPL_SETTING_DEFAULT(bool, GenElectronFromTauDecay, false);
	IMPL_SETTING_DEFAULT(bool, GenMuonFromTauDecay, false);

	IMPL_SETTING(std::string, PileupWeightFile);
	IMPL_SETTING(std::string, BTagScaleFactorFile);
	IMPL_SETTING_DEFAULT(std::string, BTagEfficiencyFile, "");

	IMPL_SETTING(std::string, EventWeight);
	IMPL_SETTING_DEFAULT(double, BaseWeight, 1.0);
	IMPL_SETTING_DEFAULT(double, GeneratorWeight, DefaultValues::UndefinedDouble);

	/// Needed by the GenDiLeptonDecayModeLFVFilter
	//IMPL_SETTING(std::string, GenDiLeptonDecayModeLFV);
	//IMPL_SETTING_DEFAULT(bool, GenDiLeptonDecayModeLFVFilterLoose, false);
	//IMPL_SETTING_DEFAULT(bool, InvertGenDiLeptonDecayModeLFV, false);

	// TMVA reader settings
	IMPL_SETTING_STRINGLIST_DEFAULT(TmvaInputQuantities, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(TmvaMethods, {});
	IMPL_SETTING_STRINGLIST_DEFAULT(TmvaWeights, {});

	// KappaCollectionsConsumer settings
	IMPL_SETTING_DEFAULT(bool, BranchGenMatchedElectrons, false);
	IMPL_SETTING_DEFAULT(bool, BranchGenMatchedMuons, false);
	IMPL_SETTING_DEFAULT(bool, BranchGenMatchedTaus, false);
	IMPL_SETTING_DEFAULT(bool, BranchGenMatchedJets, false);

	IMPL_SETTING_DEFAULT(bool, AddGenMatchedParticles, true);
	IMPL_SETTING_DEFAULT(bool, AddGenMatchedTaus, true);
	IMPL_SETTING_DEFAULT(bool, AddGenMatchedTauJets, true);

	// ZProducer
	IMPL_SETTING_DEFAULT(float, ZMass, 91.1876f);
	IMPL_SETTING(float, ZMassRange);
	IMPL_SETTING_DEFAULT(float, LowerZMassCut, 0.);
	IMPL_SETTING_DEFAULT(float, LeadingLeptonFromZLowerPtCut, 0.);
	IMPL_SETTING_DEFAULT(bool, VetoMultipleZs, true);
	IMPL_SETTING_DEFAULT(float, deltaRTolleranceForPF, 0.3);
	IMPL_SETTING_DEFAULT(float, PtTolleranceForPF, 4.);
 	IMPL_SETTING_DEFAULT(bool, MatchPairToPFCandidates, false);

	// Needed for RochMuonCorrectionsProducer
	IMPL_SETTING_DEFAULT(std::string, MuonEnergyCorrection, "none");
	IMPL_SETTING(std::string, MuonRochesterCorrectionsFile);

	// Needed by SampleStitchingWeightProducer
	IMPL_SETTING_STRINGLIST(StitchingWeights);
	IMPL_SETTING_STRINGLIST_DEFAULT(StitchingWeightsHighMass, {});

	// Needed for TauCorrectionsProducer, etc.
	IMPL_SETTING_DEFAULT(bool, CorrectOnlyRealElectrons, false);
	IMPL_SETTING_DEFAULT(bool, CorrectOnlyRealMuons, false);
	IMPL_SETTING_DEFAULT(bool, CorrectOnlyRealTaus, false);

	IMPL_SETTING_DEFAULT(std::string, DatabasePDG, "$ROOTSYS/etc/pdg_table.txt");
	IMPL_SETTING_DEFAULT(std::string, GenCollectionToPrint, "all");
	//global weight
	IMPL_SETTING_DEFAULT(float, GlobalWeight, 1.0f);
};
