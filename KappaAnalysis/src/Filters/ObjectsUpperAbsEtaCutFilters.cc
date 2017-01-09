
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"


std::string ElectronUpperAbsEtaCutsFilter::GetFilterId() const {
	return "ElectronUpperAbsEtaCutsFilter";
}

ElectronUpperAbsEtaCutsFilter::ElectronUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KElectron>(&KappaProduct::m_validElectrons)
{
}

void ElectronUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronUpperAbsEtaCuts());
}


std::string MuonUpperAbsEtaCutsFilter::GetFilterId() const {
	return "MuonUpperAbsEtaCutsFilter";
}

MuonUpperAbsEtaCutsFilter::MuonUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KMuon>(&KappaProduct::m_validMuons)
{
}

void MuonUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonUpperAbsEtaCuts());
}


std::string TauUpperAbsEtaCutsFilter::GetFilterId() const {
	return "TauUpperAbsEtaCutsFilter";
}

TauUpperAbsEtaCutsFilter::TauUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KTau>(&KappaProduct::m_validTaus)
{
}

void TauUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauUpperAbsEtaCuts());
}


std::string JetUpperAbsEtaCutsFilter::GetFilterId() const {
	return "JetUpperAbsEtaCutsFilter";
}

JetUpperAbsEtaCutsFilter::JetUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KBasicJet>(&KappaProduct::m_validJets)
{
}

void JetUpperAbsEtaCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetUpperAbsEtaCuts());
}

