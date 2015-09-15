
#include "Artus/KappaAnalysis/interface/Producers/GenTauMatchingProducers.h"


std::string RecoElectronGenTauMatchingProducer::GetProducerId() const {
	return "RecoElectronGenTauMatchingProducer";
}

RecoElectronGenTauMatchingProducer::RecoElectronGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KElectron>(&product_type::m_genTauMatchedElectrons,
	                                          &product_type::m_validElectrons,
	                                          &product_type::m_invalidElectrons,
	                                          RecoElectronGenTauMatchingProducer::TauDecayMode::E,
	                                          &setting_type::GetDeltaRMatchingRecoElectronGenTau,
	                                          &setting_type::GetInvalidateNonGenTauMatchingRecoElectrons,
						  &setting_type::GetInvalidateGenTauMatchingRecoElectrons)
{
}


std::string RecoMuonGenTauMatchingProducer::GetProducerId() const {
	return "RecoMuonGenTauMatchingProducer";
}

RecoMuonGenTauMatchingProducer::RecoMuonGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KMuon>(&product_type::m_genTauMatchedMuons,
	                                      &product_type::m_validMuons,
	                                      &product_type::m_invalidMuons,
	                                      RecoMuonGenTauMatchingProducer::TauDecayMode::M,
	                                      &setting_type::GetDeltaRMatchingRecoMuonGenTau,
	                                      &setting_type::GetInvalidateNonGenTauMatchingRecoMuons,
					      &setting_type::GetInvalidateGenTauMatchingRecoMuons)
{
}


std::string RecoTauGenTauMatchingProducer::GetProducerId() const {
	return "RecoTauGenTauMatchingProducer";
}

RecoTauGenTauMatchingProducer::RecoTauGenTauMatchingProducer() :
	GenTauMatchingProducerBase<KTau>(&product_type::m_genTauMatchedTaus,
	                                       &product_type::m_validTaus,
	                                       &product_type::m_invalidTaus,
	                                       RecoTauGenTauMatchingProducer::TauDecayMode::T,
	                                       &setting_type::GetDeltaRMatchingRecoTauGenTau,
	                                       &setting_type::GetInvalidateNonGenTauMatchingRecoTaus,
					       &setting_type::GetInvalidateGenTauMatchingRecoTaus)
{
}

