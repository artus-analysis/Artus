
#include "Artus/KappaAnalysis/interface/Producers/GenTauMatchingProducers.h"


RecoElectronGenTauMatchingProducer::RecoElectronGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KElectron>(&product_type::m_genTauMatchedElectrons,
	                                      &event_type::m_electrons,
	                                      &product_type::m_validElectrons,
	                                      &product_type::m_invalidElectrons,
	                                      RecoElectronGenTauMatchingProducer::TauDecayMode::E,
	                                      &setting_type::GetDeltaRMatchingRecoElectronGenTau,
	                                      &setting_type::GetInvalidateNonGenTauMatchingRecoElectrons,
	                                      &setting_type::GetInvalidateGenTauMatchingRecoElectrons,
	                                      &setting_type::GetMatchAllElectronsGenTau)
{
}


RecoMuonGenTauMatchingProducer::RecoMuonGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KMuon>(&product_type::m_genTauMatchedMuons,
	                                  &event_type::m_muons,
	                                  &product_type::m_validMuons,
	                                  &product_type::m_invalidMuons,
	                                  RecoMuonGenTauMatchingProducer::TauDecayMode::M,
	                                  &setting_type::GetDeltaRMatchingRecoMuonGenTau,
	                                  &setting_type::GetInvalidateNonGenTauMatchingRecoMuons,
	                                  &setting_type::GetInvalidateGenTauMatchingRecoMuons,
	                                  &setting_type::GetMatchAllMuonsGenTau)
{
}


RecoTauGenTauMatchingProducer::RecoTauGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KTau>(&product_type::m_genTauMatchedTaus,
	                                 &event_type::m_taus,
	                                 &product_type::m_validTaus,
	                                 &product_type::m_invalidTaus,
	                                 RecoTauGenTauMatchingProducer::TauDecayMode::T,
	                                 &setting_type::GetDeltaRMatchingRecoTauGenTau,
	                                 &setting_type::GetInvalidateNonGenTauMatchingRecoTaus,
	                                 &setting_type::GetInvalidateGenTauMatchingRecoTaus,
	                                 &setting_type::GetMatchAllTausGenTau)
{
}

