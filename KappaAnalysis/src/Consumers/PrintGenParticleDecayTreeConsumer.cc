
#include "Artus/KappaAnalysis/interface/Consumers/PrintGenParticleDecayTreeConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"

#include <boost/algorithm/string/replace.hpp>


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
}

void PrintGenParticleDecayTreeConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                            setting_type const& settings)
{
	assert(event.m_genParticles);
	
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();
	
	genParticleDecayTreePrinter.PrintDecayTree(event.m_genParticles);
	LOG(INFO) << "==================================================";
}

void PrintGenParticleDecayTreeConsumer::Finish(setting_type const& settings)
{
}

