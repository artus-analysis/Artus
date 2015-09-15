
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"

std::string ElectronLowerPtCutsFilter::GetFilterId() const {
	return "ElectronLowerPtCutsFilter";
}

void ElectronLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronLowerPtCuts());
}


std::string MuonLowerPtCutsFilter::GetFilterId() const {
	return "MuonLowerPtCutsFilter";
}

void MuonLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonLowerPtCuts());
}


std::string TauLowerPtCutsFilter::GetFilterId() const {
	return "TauLowerPtCutsFilter";
}

void TauLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauLowerPtCuts());
}


std::string JetLowerPtCutsFilter::GetFilterId() const {
	return "JetLowerPtCutsFilter";
}

void JetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetLowerPtCuts());
}

