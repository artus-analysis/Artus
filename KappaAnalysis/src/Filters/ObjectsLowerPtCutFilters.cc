
#include "Artus/KappaAnalysis/interface/Filters/ObjectsLowerPtCutFilters.h"


ElectronLowerPtCutsFilter::ElectronLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KElectron>(&KappaProduct::m_validElectrons)
{
}

void ElectronLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetElectronLowerPtCuts());
}


MuonLowerPtCutsFilter::MuonLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KMuon>(&KappaProduct::m_validMuons)
{
}

void MuonLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetMuonLowerPtCuts());
}


TauLowerPtCutsFilter::TauLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KTau>(&KappaProduct::m_validTaus)
{
}

void TauLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetTauLowerPtCuts());
}


JetLowerPtCutsFilter::JetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KBasicJet>(&KappaProduct::m_validJets)
{
}

void JetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetJetLowerPtCuts());
}


NonBTaggedJetLowerPtCutsFilter::NonBTaggedJetLowerPtCutsFilter() :
		LeptonLowerPtCutsFilter<KJet>(&KappaProduct::m_nonBTaggedJets)
{
}

void NonBTaggedJetLowerPtCutsFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->Initialise(settings.GetNonBTaggedJetLowerPtCuts());
}

