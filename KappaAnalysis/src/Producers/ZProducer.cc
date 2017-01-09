
#include "Artus/KappaAnalysis/interface/Producers/ZProducer.h"


std::string ZmmProducer::GetProducerId() const
{
	return "ZmmProducer";
}

ZmmProducer::ZmmProducer() :
		ZProducerBase<KMuon, KMuon>(&KappaProduct::m_validMuons)
{
}

std::string ZeeProducer::GetProducerId() const
{
	return "ZeeProducer";
}

ZeeProducer::ZeeProducer() :
		ZProducerBase<KElectron, KElectron>(&KappaProduct::m_validElectrons)
{
}

std::string ZemProducer::GetProducerId() const
{
	return "ZemProducer";
}

ZemProducer::ZemProducer() : ZProducerBase<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons, false, true)
{
}

std::string ZeemmProducer::GetProducerId() const
{
	return "ZeemmProducer";
}

ZeemmProducer::ZeemmProducer() :
		ZProducerBase<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons, true, false)
{
}

