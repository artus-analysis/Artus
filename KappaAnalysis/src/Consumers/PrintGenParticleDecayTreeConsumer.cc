
#include "Artus/KappaAnalysis/interface/Consumers/PrintGenParticleDecayTreeConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"

PrintGenParticleDecayTreeConsumer::PrintGenParticleDecayTreeConsumer() : ConsumerBase<KappaTypes>()
{
}

std::string PrintGenParticleDecayTreeConsumer::GetConsumerId() const
{
	return "PrintGenParticleDecayTreeConsumer";
}

void PrintGenParticleDecayTreeConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                            setting_type const& settings)
{
	// set LumiInfo, needs to be done here for the case running over multiple files
	product.m_hltInfo.setLumiInfo(event.m_lumiInfo);

	LOG(INFO) << "Run: " << event.m_eventInfo->nRun << ", Lumi: " << event.m_eventInfo->nLumi << ", Event: " << event.m_eventInfo->nEvent;



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
		indent += "\t";
	}

	LOG(INFO) << indent << genParticle.pdgId <<";\tp4:" << genParticle.p4;

	for (unsigned int k=0; k < genParticle.daughterIndices.size(); ++k)
	{
		long daughterIndex = genParticle.daughterIndex(k);
		KGenParticle daughterGenParticle = event.m_genParticles->at(daughterIndex);
		PrintDecayTree(daughterGenParticle, event, level+1);
	}
}
