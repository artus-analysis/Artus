#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "TDatabasePDG.h"

#include <string>


class GenParticleDecayTreePrinter {

public:
	GenParticleDecayTreePrinter(std::string pdgDatabaseFilename="");
	GenParticleDecayTreePrinter(GenParticleDecayTreePrinter& genParticleDecayTreePrinter);
	~GenParticleDecayTreePrinter();
	
	void InitPDGDatabase(std::string pdgDatabaseFilename);
	TDatabasePDG* GetPdgDatabase();
	
	void PrintGenParticlesDecayTree(std::vector<KGenParticle>* genParticles, int currentIndex=-1, unsigned int currentLevel=0) const;
	void PrintLHEParticlesDecayTree(KLHEParticles* lheParticles, int currentIndex=-1, unsigned int currentLevel=0) const;

private:
	TDatabasePDG* m_databasePDG = nullptr;

	static std::string GetIndent(unsigned int level);
};
