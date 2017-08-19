
#include "Artus/KappaAnalysis/interface/Filters/ObjectsUpperAbsEtaCutFilters.h"


std::string ElectronUpperAbsEtaCutsFilter::GetFilterId() const
{
	return "ElectronUpperAbsEtaCutsFilter";
}

ElectronUpperAbsEtaCutsFilter::ElectronUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KElectron>(&product_type::m_validElectrons)
{
}

void ElectronUpperAbsEtaCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetElectronUpperAbsEtaCuts());
}


std::string MuonUpperAbsEtaCutsFilter::GetFilterId() const
{
	return "MuonUpperAbsEtaCutsFilter";
}

MuonUpperAbsEtaCutsFilter::MuonUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KMuon>(&product_type::m_validMuons)
{
}

void MuonUpperAbsEtaCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetMuonUpperAbsEtaCuts());
}


std::string TauUpperAbsEtaCutsFilter::GetFilterId() const
{
	return "TauUpperAbsEtaCutsFilter";
}

TauUpperAbsEtaCutsFilter::TauUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KTau>(&product_type::m_validTaus)
{
}

void TauUpperAbsEtaCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetTauUpperAbsEtaCuts());
}


std::string JetUpperAbsEtaCutsFilter::GetFilterId() const {
	return "JetUpperAbsEtaCutsFilter";
}

JetUpperAbsEtaCutsFilter::JetUpperAbsEtaCutsFilter() :
		LeptonUpperAbsEtaCutsFilter<KBasicJet>(&product_type::m_validJets)
{
}

void JetUpperAbsEtaCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetJetUpperAbsEtaCuts());
}

