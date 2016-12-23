
#include "Artus/KappaAnalysis/interface/Producers/ZProducer.h"


ZmmProducer::ZmmProducer() :
		ZProducerBase<KMuon, KMuon>(&KappaProduct::m_validMuons)
{
}

ZeeProducer::ZeeProducer() :
		ZProducerBase<KElectron, KElectron>(&KappaProduct::m_validElectrons)
{
}

ZemProducer::ZemProducer() : ZProducerBase<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons, false, true)
{
}

ZeemmProducer::ZeemmProducer() :
		ZProducerBase<KElectron, KMuon>(&KappaProduct::m_validElectrons, &KappaProduct::m_validMuons, true, false)
{
}

