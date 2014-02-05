
#include <iostream>

#include "Artus/Configuration/interface/RootEnvironment.h"


RootEnvironment::RootEnvironment(const ArtusConfig & artusConfig) :
	m_rootFileName(artusConfig.GetOutputPath())
{
	m_rootFile = new TFile(m_rootFileName.c_str(), "RECREATE");
	std::cout << "Output file \"" << m_rootFileName << "\" created." << std::endl;
	
	artusConfig.SaveConfig(m_rootFile);
}

void RootEnvironment::Close() {
	if(m_rootFile) {
		m_rootFile->Close();
		std::cout << "Output file \"" << m_rootFileName << "\" closed." << std::endl;
		delete m_rootFile;
		m_rootFile = 0;
	}
}
