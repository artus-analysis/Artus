/* Copyright (c) 2010 - All Rights Reserved
 *   Andreas Oehler <andreasoehler@gmx.net>
 *   Armin Burgmeier <burgmeier@ekp.uni-karlsruhe.de>
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef KAPPA_FILEINTERFACE_H
#define KAPPA_FILEINTERFACE_H

#include "Artus/KappaTools/interface/ProgressMonitor.h"
#include "Artus/KappaTools/interface/IOHelper.h"
#include "Artus/KappaTools/interface/FileInterfaceBase.h"

class FileInterface : public FileInterfaceBase
{
public:
	FileInterface(std::vector<std::string> files, bool shuffle = false, int verbose = 2);
	~FileInterface();
	using FileInterfaceBase::GetEvent;

	// Functions for getting metadata objects
	template<typename T>
	T *GetEvent(run_id run, lumi_id lumi);
	template<typename T>
	inline T *GetEvent(KEventInfo *info_event);

	// Get lumi list
	std::vector<std::pair<run_id, lumi_id> > GetRunLumis() const;
	bool isCompatible(unsigned int minRun, unsigned int maxRun);

	TChain eventdata;
	
private:
	TChain lumidata;
	TChain rundata;
	
	int verbosity;

	std::map<std::pair<run_id, lumi_id>, KGenRunInfo> lumimap_mc;
	std::map<std::pair<run_id, lumi_id>, KLumiInfo> lumimap_std;
	std::map<std::pair<run_id, lumi_id>, KDataLumiInfo> lumimap_data;

	template<typename T>
	std::map<std::pair<run_id, lumi_id>, T> GetLumis();
};

#include "FileInterface.hxx"

#endif
