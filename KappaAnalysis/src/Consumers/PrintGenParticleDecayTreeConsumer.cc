
#include "Artus/KappaAnalysis/interface/Consumers/PrintGenParticleDecayTreeConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>


PrintGenParticleDecayTreeConsumer::PrintGenParticleDecayTreeConsumer() : ConsumerBase<KappaTypes>()
{
}

std::string PrintGenParticleDecayTreeConsumer::GetConsumerId() const
{
	return "PrintGenParticleDecayTreeConsumer";
}

void PrintGenParticleDecayTreeConsumer::Init(setting_type const& settings)
{
	genParticleDecayTreePrinter.InitPDGDatabase(settings.GetDatabasePDG());
	genCollectionToPrint = KappaEnumTypes::ToGenCollectionToPrint(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetGenCollectionToPrint())));
}

void PrintGenParticleDecayTreeConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings)
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

void PrintGenParticleDecayTreeConsumer::Finish(setting_type const& settings)
{
}

