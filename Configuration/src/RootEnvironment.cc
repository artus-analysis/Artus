
#include <iostream>

#include "Artus/Configuration/interface/RootEnvironment.h"


RootEnvironment::RootEnvironment(const ArtusConfig & artusConfig) :
	m_rootFileName(artusConfig.GetOutputPath())
{
	m_rootFile = new TFile(m_rootFileName.c_str(), "RECREATE");
	LOG(INFO) << "Output file \"" << m_rootFileName << "\" created.";

	artusConfig.SaveConfig(m_rootFile);
}

RootEnvironment::~RootEnvironment() {
	Close();
}

void RootEnvironment::Close() {
	if(m_rootFile) {
		m_rootFile->Close();
		LOG(INFO) << "Output file \"" << m_rootFileName << "\" closed.";
		delete m_rootFile;
		m_rootFile = nullptr;
	}
}
