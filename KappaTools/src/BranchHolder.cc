/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/BranchHolder.h"
#include <iostream>

BranchHolder::BranchHolder(TTree *_tree, const std::string _bname, std::string cname, const bool check)
	: ptr(0), tree(_tree), bname(_bname)
{
	TBranch *branch = tree->GetBranch(bname.c_str());
	if (branch)
	{
		if (cname == "")
			cname = branch->GetClassName();
		// Consistency check
		if (CheckBranch(branch, cname, check))
		{
			// This complicated SetBranchAddress call is neccessary since
			// the simple tree->SetBranchAddress(bname.c_str(), &ptr) call
			// triggers a bug in ROOT, which performs a type check on a
			// zero initialized pointer. Found and fixed by Fred S.
			TClass *branchClass;
			EDataType branchType;
			if (branch->GetExpectedType(branchClass, branchType) == 0)
			{
				// Allocate correct instance and set pointer
				ptr = branchClass->New();
				tree->SetBranchAddress(bname.c_str(), &ptr, branchClass, branchType, true);
			}
			else 
				std::cerr << "Unable to get branch information: " << bname << std::endl;
		}
	}
	else 
		std::cerr << "Requested branch not found: " << bname << std::endl;
}

std::string BranchHolder::ClassName()
{
	TBranch *branch = tree->GetBranch(bname.c_str());
	if (branch)
		return branch->GetClassName();
	return "";
}

void BranchHolder::UpdateTree(TTree *newTree)
{
	if (ptr)
	{
		TBranch *branch = tree->GetBranch(bname.c_str());
		tree->ResetBranchAddress(branch);
		newTree->SetBranchAddress(bname.c_str(), &ptr);
	}
	tree = newTree;
}

BranchHolder::~BranchHolder()
{
	if (ptr)
	{
		TBranch *branch = tree->GetBranch(bname.c_str());
		tree->ResetBranchAddress(branch);
		TClass::GetClass(branch->GetClassName())->GetDelete()(ptr);
	}
}
