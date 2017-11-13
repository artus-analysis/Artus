/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_BRANCHHOLDER_H
#define KAPPA_BRANCHHOLDER_H

#include <TTree.h>
#include "Artus/KappaTools/interface/Directory.h"

struct BranchHolder
{
	BranchHolder(TTree *_tree, const std::string _bname, std::string cname = "", const bool check = true);
	~BranchHolder();

	std::string ClassName();
	void UpdateTree(TTree *newTree);

	template<typename T>
	T *Get()
	{
		return static_cast<T*>(ptr);
	}
	void *ptr;
private:
	TTree *tree;
	std::string bname;
};

#endif
