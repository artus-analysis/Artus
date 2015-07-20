
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"


std::string ElectronUpperAbsEtaCutsFilter::GetFilterId() const {
	return "ElectronUpperAbsEtaCutsFilter";
}

void ElectronUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronUpperAbsEtaCuts());
}


std::string MuonUpperAbsEtaCutsFilter::GetFilterId() const {
	return "MuonUpperAbsEtaCutsFilter";
}

void MuonUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonUpperAbsEtaCuts());
}


std::string TauUpperAbsEtaCutsFilter::GetFilterId() const {
	return "TauUpperAbsEtaCutsFilter";
}

void TauUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauUpperAbsEtaCuts());
}


std::string JetUpperAbsEtaCutsFilter::GetFilterId() const {
	return "JetUpperAbsEtaCutsFilter";
}

void JetUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetUpperAbsEtaCuts());
}

