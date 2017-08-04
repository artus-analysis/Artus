
#include "Artus/KappaAnalysis/interface/Producers/PrintGenParticleDecayTreeProducer.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>


std::string PrintGenParticleDecayTreeProducer::GetProducerId() const
{
	return "PrintGenParticleDecayTreeProducer";
}

void PrintGenParticleDecayTreeProducer::Init(KappaSettings const& settings)
{
	genParticleDecayTreePrinter.InitPDGDatabase(settings.GetDatabasePDG());
	genCollectionToPrint = KappaEnumTypes::ToGenCollectionToPrint(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetGenCollectionToPrint())));
}

void PrintGenParticleDecayTreeProducer::Produce(KappaEvent const& event, KappaProduct & product, KappaSettings const& settings) const
{
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();
	
	if (event.m_genParticles &&
	    ((genCollectionToPrint == KappaEnumTypes::GenCollectionToPrint::ALL) ||
	     (genCollectionToPrint == KappaEnumTypes::GenCollectionToPrint::GEN)))
	{
		assert(event.m_genParticles);
		genParticleDecayTreePrinter.PrintGenParticlesDecayTree(event.m_genParticles);
		LOG(INFO) << "==================================================";
	}
	
	if (event.m_lheParticles &&
	    ((genCollectionToPrint == KappaEnumTypes::GenCollectionToPrint::ALL) ||
	     (genCollectionToPrint == KappaEnumTypes::GenCollectionToPrint::LHE)))
	{
		assert(event.m_lheParticles);
		genParticleDecayTreePrinter.PrintLHEParticlesDecayTree(event.m_lheParticles);
		LOG(INFO) << "==================================================";
	}
}

