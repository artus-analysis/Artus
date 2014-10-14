
#include "Artus/KappaAnalysis/interface/Producers/GenMatchingProducers.h"


std::string ElectronGenMatchingProducer::GetProducerId() const {
	return "ElectronGenMatchingProducer";
}

ElectronGenMatchingProducer::ElectronGenMatchingProducer() :
	GenMatchingProducerBase<KDataElectron>(&product_type::m_genMatchedElectrons,
	                                       &product_type::m_validElectrons,
	                                       &product_type::m_invalidElectrons,
	                                       ElectronGenMatchingProducer::TauDecayMode::E,
	                                       &setting_type::GetDeltaRGenMatchingElectrons,
	                                       &setting_type::GetInvalidateNonGenMatchingElectrons,
	                                       &setting_type::GetMatchingAlgorithmusJets)
{
}


std::string MuonGenMatchingProducer::GetProducerId() const {
	return "MuonGenMatchingProducer";
}

MuonGenMatchingProducer::MuonGenMatchingProducer() :
	GenMatchingProducerBase<KDataMuon>(&product_type::m_genMatchedMuons,
	                                   &product_type::m_validMuons,
	                                   &product_type::m_invalidMuons,
	                                   MuonGenMatchingProducer::TauDecayMode::M,
	                                   &setting_type::GetDeltaRGenMatchingMuons,
	                                   &setting_type::GetInvalidateNonGenMatchingMuons,
	                                   &setting_type::GetMatchingAlgorithmusJets)
{
}


std::string TauGenMatchingProducer::GetProducerId() const {
	return "TauGenMatchingProducer";
}

TauGenMatchingProducer::TauGenMatchingProducer() :
	GenMatchingProducerBase<KDataPFTau>(&product_type::m_genMatchedTaus,
	                                    &product_type::m_validTaus,
	                                    &product_type::m_invalidTaus,
	                                    TauGenMatchingProducer::TauDecayMode::T,
	                                    &setting_type::GetDeltaRGenMatchingTaus,
	                                    &setting_type::GetInvalidateNonGenMatchingTaus,
	                                    &setting_type::GetMatchingAlgorithmusJets)
{
}


std::string JetGenMatchingProducer::GetProducerId() const {
	return "JetGenMatchingProducer";
}

JetGenMatchingProducer::JetGenMatchingProducer() :
	GenMatchingProducerBase<KDataPFJet>(&product_type::m_genMatchedJets,
	                                    &product_type::m_validJets,
	                                    &product_type::m_invalidJets,
	                                    JetGenMatchingProducer::TauDecayMode::NONE,
	                                    &setting_type::GetDeltaRGenMatchingJets,
	                                    &setting_type::GetInvalidateNonGenMatchingJets,
	                                    &setting_type::GetMatchingAlgorithmusJets)
{
}

