
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
	std::string filename = settings.GetDatabasePDG();
	boost::algorithm::replace_first(filename, "$ROOTSYS", getenv("ROOTSYS"));
	LOG(DEBUG) << "Read PDG database from \"" << filename << "\"...";
	m_databasePDG.ReadPDGTable(filename.c_str());
}

void PrintGenParticleDecayTreeConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                            setting_type const& settings)
{
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();

	for (unsigned int j = 0; j < event.m_genParticles->size(); ++j)
	{
		KGenParticle currentGenParticle = event.m_genParticles->at(j);

		if (currentGenParticle.pdgId == DefaultValues::pdgIdProton)
		{
			PrintDecayTree(currentGenParticle, event, 0);
		}
	}
	LOG(INFO) << "==================================================";
}

void PrintGenParticleDecayTreeConsumer::Finish(setting_type const& settings)
{
}

void PrintGenParticleDecayTreeConsumer::PrintDecayTree(KGenParticle const& genParticle, event_type const& event, int level) const
{
	std::string indent = "";
	for (int i = 0; i < level; ++i)
	{
		if (i == level - 1)
		{
			indent += "|---";
		}
		else if (i < level - 1)
		{
			indent += "    ";
		}
		else if (i > level - 1)
		{
			indent += "----";
		}
		
	}
	
	std::string name = "";
	TParticlePDG* pdgParticle = m_databasePDG.GetParticle(genParticle.pdgId);
	if (pdgParticle)
	{
		name = pdgParticle->GetName();
	}
	LOG(INFO) << indent << "-> " << name << ", PDG ID = " << genParticle.pdgId << ": p4 = " << genParticle.p4;

	for (std::vector<unsigned int>::const_iterator daughterIndex = genParticle.daughterIndices.begin();
	     daughterIndex != genParticle.daughterIndices.end(); ++daughterIndex)
	{
		KGenParticle daughterGenParticle = event.m_genParticles->at(*daughterIndex);
		PrintDecayTree(daughterGenParticle, event, level+1);
	}
}
