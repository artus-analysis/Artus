
#include "Artus/Utility/interface/ArtusLogging.h"
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTreePrinter.h"

#include <boost/algorithm/string/replace.hpp>


GenParticleDecayTreePrinter::GenParticleDecayTreePrinter(std::string pdgDatabaseFilename)
{
	InitPDGDatabase(pdgDatabaseFilename);
}

GenParticleDecayTreePrinter::GenParticleDecayTreePrinter(GenParticleDecayTreePrinter& genParticleDecayTreePrinter)
{
	m_databasePDG = genParticleDecayTreePrinter.GetPdgDatabase();
}

GenParticleDecayTreePrinter::~GenParticleDecayTreePrinter()
{
	if (m_databasePDG)
	{
		delete m_databasePDG;
		m_databasePDG = nullptr;
	}
}

void GenParticleDecayTreePrinter::InitPDGDatabase(std::string pdgDatabaseFilename)
{
	if (! pdgDatabaseFilename.empty())
	{
		if (m_databasePDG)
		{
			delete m_databasePDG;
			m_databasePDG = nullptr;
		}
		m_databasePDG = new TDatabasePDG();
		boost::algorithm::replace_first(pdgDatabaseFilename, "$ROOTSYS", getenv("ROOTSYS"));
		LOG(DEBUG) << "Read PDG database from \"" << pdgDatabaseFilename << "\"...";
		m_databasePDG->ReadPDGTable(pdgDatabaseFilename.c_str());
	}
}

TDatabasePDG* GenParticleDecayTreePrinter::GetPdgDatabase()
{
	return m_databasePDG;
}

void GenParticleDecayTreePrinter::PrintDecayTree(std::vector<KGenParticle>* genParticles, int currentIndex, unsigned int currentLevel) const
{
	if (currentIndex < 0)
	{
		for (unsigned int genParticleIndex = 0; genParticleIndex < genParticles->size(); ++genParticleIndex)
		{
			if (genParticles->at(genParticleIndex).pdgId == 2212)
			{
				PrintDecayTree(genParticles, genParticleIndex, 0);
			}
		}
	}
	else
	{
		std::string indent = "";
		for (unsigned int levelIndex = 0; levelIndex < currentLevel; ++levelIndex)
		{
			if (levelIndex == currentLevel - 1)
			{
				indent += "|---";
			}
			else if (levelIndex < currentLevel - 1)
			{
				indent += "    ";
			}
			else if (levelIndex > currentLevel - 1)
			{
				indent += "----";
			}
	
		}

		KGenParticle& genParticle = genParticles->at(currentIndex);
		std::string name = "";
		TParticlePDG* pdgParticle = m_databasePDG->GetParticle(genParticle.pdgId);
		if (pdgParticle)
		{
			name = pdgParticle->GetName();
		}
		LOG(INFO) << indent << "-> " << name << ", PDG ID = " << genParticle.pdgId << ", status = " << genParticle.status() << ", p4 = " << genParticle.p4 << ", kappa index = " << currentIndex;

		for (std::vector<unsigned int>::const_iterator daughterIndex = genParticle.daughterIndices.begin();
			 daughterIndex != genParticle.daughterIndices.end(); ++daughterIndex)
		{
			PrintDecayTree(genParticles, *daughterIndex, currentLevel+1);
		}
	}
}

