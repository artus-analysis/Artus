
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

void GenParticleDecayTreePrinter::PrintGenParticlesDecayTree(std::vector<KGenParticle>* genParticles, int currentIndex, unsigned int currentLevel) const
{
	if (currentIndex < 0)
	{
		bool foundgenProton = false;
		int genTauIndex_1 = DefaultValues::UndefinedInt; // save tau indices if they are no protons on gen level (used in embedding)
		int genTauIndex_2 = DefaultValues::UndefinedInt;
		for (unsigned int genParticleIndex = 0; genParticleIndex < genParticles->size(); ++genParticleIndex)
		{
			if (genParticles->at(genParticleIndex).pdgId == 2212)
			{
				foundgenProton = true;
				PrintGenParticlesDecayTree(genParticles, genParticleIndex, 0);
			}
			else if (genParticles->at(genParticleIndex).pdgId == DefaultValues::pdgIdTau && !foundgenProton && genTauIndex_1 == DefaultValues::UndefinedInt)
			{
				genTauIndex_1 = genParticleIndex;
			}
			else if (genParticles->at(genParticleIndex).pdgId == -DefaultValues::pdgIdTau && !foundgenProton && genTauIndex_2 == DefaultValues::UndefinedInt)
			{
				genTauIndex_2 = genParticleIndex;
			}
		}
		if(!foundgenProton && genTauIndex_1 != DefaultValues::UndefinedInt) PrintGenParticlesDecayTree(genParticles, genTauIndex_1, 0);
		if(!foundgenProton && genTauIndex_2 != DefaultValues::UndefinedInt) PrintGenParticlesDecayTree(genParticles, genTauIndex_2, 0);
	}
	else
	{
		std::string indent = GenParticleDecayTreePrinter::GetIndent(currentLevel);

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
			PrintGenParticlesDecayTree(genParticles, *daughterIndex, currentLevel+1);
		}
	}
}

void GenParticleDecayTreePrinter::PrintLHEParticlesDecayTree(KLHEParticles* lheParticles, int currentIndex, unsigned int currentLevel) const
{
	if (currentIndex < 0)
	{
		for (unsigned int lheParticleIndex = 0; lheParticleIndex < lheParticles->particles.size(); ++lheParticleIndex)
		{
			if (true) // (lheParticles->particles.at(lheParticleIndex).p4.Pt() < 1e-3)
			{
				PrintLHEParticlesDecayTree(lheParticles, lheParticleIndex, 0);
			}
		}
	}
	else
	{
		std::string indent = GenParticleDecayTreePrinter::GetIndent(currentLevel);

		KLHEParticle& lheParticle = lheParticles->particles.at(currentIndex);
		std::string name = "";
		TParticlePDG* pdgParticle = m_databasePDG->GetParticle(lheParticle.pdgId);
		if (pdgParticle)
		{
			name = pdgParticle->GetName();
		}
		LOG(INFO) << indent << "-> " << name << ", PDG ID = " << lheParticle.pdgId << ", status = " << lheParticle.status << ", p4 = " << lheParticle.p4 << ", kappa index = " << currentIndex; // << ", daughter indices = " << lheParticle.firstLastMotherIDs.first << " - " << lheParticle.firstLastMotherIDs.second;

		/*for (int daughterIndex = lheParticle.firstLastMotherIDs.first; daughterIndex <= lheParticle.firstLastMotherIDs.second; ++daughterIndex)
		{
			PrintLHEParticlesDecayTree(lheParticles, daughterIndex, currentLevel+1);
		}*/
	}
}

std::string GenParticleDecayTreePrinter::GetIndent(unsigned int level)
{
	std::string indent = "";
	for (unsigned int levelIndex = 0; levelIndex < level; ++levelIndex)
	{
		if (levelIndex == level - 1)
		{
			indent += "|---";
		}
		else if (levelIndex < level - 1)
		{
			indent += "    ";
		}
		else if (levelIndex > level - 1)
		{
			indent += "----";
		}

	}
	return indent;
}

