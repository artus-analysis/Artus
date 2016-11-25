
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"


void ElectronLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronLowerPtCuts());
}


void MuonLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonLowerPtCuts());
}


void TauLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauLowerPtCuts());
}


void JetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetLowerPtCuts());
}


void NonBTaggedJetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetNonBTaggedJetLowerPtCuts());
}

