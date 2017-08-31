
#include "Artus/KappaAnalysis/interface/Producers/ZProducer.h"


std::string ZmmProducer::GetProducerId() const
{
	return "ZmmProducer";
}

ZmmProducer::ZmmProducer() :
		ZProducerBase<KMuon, KMuon>(&product_type::m_validMuons)
{
}

std::string ZeeProducer::GetProducerId() const
{
	return "ZeeProducer";
}

ZeeProducer::ZeeProducer() :
		ZProducerBase<KElectron, KElectron>(&product_type::m_validElectrons)
{
}

std::string ZemProducer::GetProducerId() const
{
	return "ZemProducer";
}

ZemProducer::ZemProducer() : ZProducerBase<KElectron, KMuon>(&product_type::m_validElectrons, &product_type::m_validMuons, false, true)
{
}

std::string ZeemmProducer::GetProducerId() const
{
	return "ZeemmProducer";
}

ZeemmProducer::ZeemmProducer() :
		ZProducerBase<KElectron, KMuon>(&product_type::m_validElectrons, &product_type::m_validMuons, true, false)
{
}

