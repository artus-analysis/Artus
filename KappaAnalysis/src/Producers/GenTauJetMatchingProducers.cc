
#include "Artus/KappaAnalysis/interface/Producers/GenTauJetMatchingProducers.h"


std::string RecoElectronGenTauJetMatchingProducer::GetProducerId() const {
	return "RecoElectronGenTauJetMatchingProducer";
}

RecoElectronGenTauJetMatchingProducer::RecoElectronGenTauJetMatchingProducer() :
	GenTauJetMatchingProducerBase<KElectron>(&product_type::m_genTauJetMatchedElectrons,
	                                          &product_type::m_validElectrons,
	                                          &product_type::m_invalidElectrons,
	                                          RecoElectronGenTauJetMatchingProducer::TauDecayMode::E,
	                                          &setting_type::GetDeltaRMatchingRecoElectronGenTauJet,
	                                          &setting_type::GetInvalidateNonGenTauJetMatchingRecoElectrons,
						  &setting_type::GetInvalidateGenTauJetMatchingRecoElectrons)
{
}


std::string RecoMuonGenTauJetMatchingProducer::GetProducerId() const {
	return "RecoMuonGenTauJetMatchingProducer";
}

RecoMuonGenTauJetMatchingProducer::RecoMuonGenTauJetMatchingProducer() :
	GenTauJetMatchingProducerBase<KMuon>(&product_type::m_genTauJetMatchedMuons,
	                                      &product_type::m_validMuons,
	                                      &product_type::m_invalidMuons,
	                                      RecoMuonGenTauJetMatchingProducer::TauDecayMode::M,
	                                      &setting_type::GetDeltaRMatchingRecoMuonGenTauJet,
	                                      &setting_type::GetInvalidateNonGenTauJetMatchingRecoMuons,
					      &setting_type::GetInvalidateGenTauJetMatchingRecoMuons)
{
}


std::string RecoTauGenTauJetMatchingProducer::GetProducerId() const {
	return "RecoTauGenTauJetMatchingProducer";
}

RecoTauGenTauJetMatchingProducer::RecoTauGenTauJetMatchingProducer() :
	GenTauJetMatchingProducerBase<KTau>(&product_type::m_genTauJetMatchedTaus,
	                                       &product_type::m_validTaus,
	                                       &product_type::m_invalidTaus,
	                                       RecoTauGenTauJetMatchingProducer::TauDecayMode::T,
	                                       &setting_type::GetDeltaRMatchingRecoTauGenTauJet,
	                                       &setting_type::GetInvalidateNonGenTauJetMatchingRecoTaus,
					       &setting_type::GetInvalidateGenTauJetMatchingRecoTaus)
{
}

