/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_DIRECTORY_H
#define KAPPA_DIRECTORY_H

#include <vector>
#include <map>
#include <TDirectory.h>
#include <TTree.h>
#include <TFile.h>
#include <TKey.h>

class ROOTGuard
{
public:
	ROOTGuard() : dir(gDirectory), file(gFile) {}
	~ROOTGuard()
	{
		gDirectory = dir;
		gFile = file;
	}
private:
	TDirectory *dir;
	TFile *file;
};

std::map<std::string, TObject*> GetDirObjectsMap(TDirectory *dir);

template<typename T>
std::map<std::string, T*> GetDirObjectsMap(TDirectory *dir)
{
	std::map<std::string, T*> result;
	std::map<std::string, TObject*> objects = GetDirObjectsMap(dir);
	for (std::map<std::string, TObject*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
		if (it->second->InheritsFrom(T::Class()))
			result[it->first] = dynamic_cast<T*>(it->second);
	return result;
}

std::vector<std::string> TreeObjects(TTree* chain, const std::string cname, const bool inherited = false);
bool CheckBranch(TBranch *branch, const std::string cname, bool checkDict);
bool CheckType(const std::string req, const std::string cur);

#endif
