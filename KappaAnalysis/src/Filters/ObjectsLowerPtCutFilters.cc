
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"

std::string ElectronLowerPtCutsFilter::GetFilterId() const {
	return "ElectronLowerPtCutsFilter";
}

ElectronLowerPtCutsFilter::ElectronLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KElectron>(&KappaProduct::m_validElectrons)
{
}

void ElectronLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronLowerPtCuts());
}


std::string MuonLowerPtCutsFilter::GetFilterId() const {
	return "MuonLowerPtCutsFilter";
}

MuonLowerPtCutsFilter::MuonLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KMuon>(&KappaProduct::m_validMuons)
{
}

void MuonLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonLowerPtCuts());
}


std::string TauLowerPtCutsFilter::GetFilterId() const {
	return "TauLowerPtCutsFilter";
}

TauLowerPtCutsFilter::TauLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KTau>(&KappaProduct::m_validTaus)
{
}

void TauLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauLowerPtCuts());
}


std::string JetLowerPtCutsFilter::GetFilterId() const {
	return "JetLowerPtCutsFilter";
}

JetLowerPtCutsFilter::JetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KBasicJet>(&KappaProduct::m_validJets)
{
}

void JetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetLowerPtCuts());
}


std::string NonBTaggedJetLowerPtCutsFilter::GetFilterId() const {
	return "NonBTaggedJetLowerPtCutsFilter";
}

NonBTaggedJetLowerPtCutsFilter::NonBTaggedJetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KJet>(&KappaProduct::m_nonBTaggedJets)
{
}

void NonBTaggedJetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetNonBTaggedJetLowerPtCuts());
}

