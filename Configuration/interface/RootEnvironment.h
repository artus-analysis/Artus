
#pragma once

#include "Artus/Configuration/interface/ArtusConfig.h"

#include <TFile.h>

#include <string>


class RootEnvironment {
public:
	explicit RootEnvironment(const ArtusConfig& artusConfig);
	~RootEnvironment() { Close(); };
	
	inline TFile* GetRootFile() const { return m_rootFile; };
	void Close();
	
private:
	TFile* m_rootFile;
	std::string m_rootFileName;
};

