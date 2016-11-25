
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"


void ElectronUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronUpperAbsEtaCuts());
}


void MuonUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonUpperAbsEtaCuts());
}


void TauUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauUpperAbsEtaCuts());
}


void JetUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetUpperAbsEtaCuts());
}

