
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"


ElectronUpperAbsEtaCutsFilter::ElectronUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KElectron>(&KappaProduct::m_validElectrons)
{
}

void ElectronUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronUpperAbsEtaCuts());
}


MuonUpperAbsEtaCutsFilter::MuonUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KMuon>(&KappaProduct::m_validMuons)
{
}

void MuonUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonUpperAbsEtaCuts());
}


TauUpperAbsEtaCutsFilter::TauUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KTau>(&KappaProduct::m_validTaus)
{
}

void TauUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauUpperAbsEtaCuts());
}


JetUpperAbsEtaCutsFilter::JetUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KBasicJet>(&KappaProduct::m_validJets)
{
}

void JetUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetUpperAbsEtaCuts());
}

