
#include "Artus/KappaAnalysis/interface/Producers/PrintGenParticleDecayTreeProducer.h"

std::string PrintGenParticleDecayTreeProducer::GetProducerId() const
{
	return "PrintGenParticleDecayTreeProducer";
}

void PrintGenParticleDecayTreeProducer::Init(KappaSettings const& settings)
{
	genParticleDecayTreePrinter.InitPDGDatabase(settings.GetDatabasePDG());
}

void PrintGenParticleDecayTreeProducer::Produce(KappaEvent const& event, KappaProduct & product, KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();
	
	genParticleDecayTreePrinter.PrintDecayTree(event.m_genParticles);
	LOG(INFO) << "==================================================";
}

