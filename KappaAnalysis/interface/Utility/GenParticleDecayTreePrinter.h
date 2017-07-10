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
	
	void PrintDecayTree(std::vector<KGenParticle>* genParticles, int currentIndex=-1, unsigned int currentLevel=0) const;

private:
	TDatabasePDG* m_databasePDG = nullptr;

};
