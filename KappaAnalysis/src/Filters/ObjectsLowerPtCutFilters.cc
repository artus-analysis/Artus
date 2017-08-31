
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"

std::string ElectronLowerPtCutsFilter::GetFilterId() const
{
	return "ElectronLowerPtCutsFilter";
}

ElectronLowerPtCutsFilter::ElectronLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KElectron>(&product_type::m_validElectrons)
{
}

void ElectronLowerPtCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetElectronLowerPtCuts());
}


std::string MuonLowerPtCutsFilter::GetFilterId() const {
	return "MuonLowerPtCutsFilter";
}

MuonLowerPtCutsFilter::MuonLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KMuon>(&product_type::m_validMuons)
{
}

void MuonLowerPtCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetMuonLowerPtCuts());
}


std::string TauLowerPtCutsFilter::GetFilterId() const
{
	return "TauLowerPtCutsFilter";
}

TauLowerPtCutsFilter::TauLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KTau>(&product_type::m_validTaus)
{
}

void TauLowerPtCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetTauLowerPtCuts());
}


std::string JetLowerPtCutsFilter::GetFilterId() const
{
	return "JetLowerPtCutsFilter";
}

JetLowerPtCutsFilter::JetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KBasicJet>(&product_type::m_validJets)
{
}

void JetLowerPtCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetJetLowerPtCuts());
}


std::string NonBTaggedJetLowerPtCutsFilter::GetFilterId() const
{
	return "NonBTaggedJetLowerPtCutsFilter";
}

NonBTaggedJetLowerPtCutsFilter::NonBTaggedJetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KJet>(&product_type::m_nonBTaggedJets)
{
}

void NonBTaggedJetLowerPtCutsFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->Initialise(settings.GetNonBTaggedJetLowerPtCuts());
}

