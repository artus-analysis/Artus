/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/Directory.h"
#include <assert.h>
#include "TDataMember.h"
#include "Artus/KappaTools/interface/IOHelper.h"
#if CMSSW_MAJOR_VERSION >= 8
#include "TVirtualCollectionProxy.h"
#endif



std::map<std::string, TObject*> GetDirObjectsMap(TDirectory *dir)
{
	std::map<std::string, TObject*> result;
	TIter iter_mem(dir->GetList());
	while (TObject *obj = dynamic_cast<TObject*>(iter_mem()))
		result[obj->GetName()] = obj;
	TIter iter_disk(dir->GetListOfKeys());
	while (TKey *key = dynamic_cast<TKey*>(iter_disk()))
		if (result[key->GetName()] == 0)
			result[key->GetName()] = key->ReadObj();
	return result;
}

std::vector<std::string> TreeObjects(TTree* chain, const std::string cname, const bool inherited)
{
	std::vector<std::string> result;
	TObjArray *branches = chain->GetListOfBranches();
	if (branches == 0)
		return result;
	TClass *req = TClass::GetClass(cname.c_str());
	std::string reqName = req->GetName();
	for (int i = 0; i < branches->GetEntries(); ++i)
	{
		TBranch *b = dynamic_cast<TBranch*>(branches->At(i));
		TClass *cur = TClass::GetClass(b->GetClassName());
		if ((cur == req) || (inherited && cur->InheritsFrom(req)))
			result.push_back(b->GetName());
	}
	return result;
}

void GetListOfAllBranches(TBranch* branch, std::set<std::string> &result)
{
	TObjArray *lmBranch = branch->GetListOfBranches();
	for (int i = 0; i < lmBranch->GetEntries(); ++i)
	{
		TBranch *subbranch = dynamic_cast<TBranch*>(lmBranch->At(i));
		assert(subbranch != NULL);
		if (subbranch->GetListOfBranches()->GetEntries() > 0)
			GetListOfAllBranches(subbranch, result);
		else
			result.insert(lmBranch->At(i)->GetName());
	}
}

void GetListOfAllDataMembers(TClass *cls, std::set<std::string> &result, std::string prefix = "")
{
	if (cls == 0)
		return;
	TList *lMembers = cls->GetListOfDataMembers();
	if (lMembers)
		for (int i = 0; i < lMembers->GetEntries(); ++i)
		{
			TDataMember *m = dynamic_cast<TDataMember*>(lMembers->At(i));
			std::string name = prefix + m->GetName();
			if (m->IsBasic() || m->IsSTLContainer())
			{
				for (int j = 0; j < m->GetArrayDim(); ++j)
					name += "[" + KappaTools::str(m->GetMaxIndex(j)) + "]";
				result.insert(name);
			}
			else
				GetListOfAllDataMembers(TClass::GetClass(m->GetFullTypeName()), result, name + ".");
		}
	TList *lBases = cls->GetListOfBases();
	if (lBases)
		for (int i = 0; i < lBases->GetEntries(); ++i)
			GetListOfAllDataMembers(TClass::GetClass(lBases->At(i)->GetName()), result, prefix);
}

bool CheckType(const std::string req, const std::string cur)
{
	assert(req != "");
	assert(cur != "");
	TClass *classRequest = TClass::GetClass(req.c_str());
	TClass *classCurrent = TClass::GetClass(cur.c_str());
	
	#if CMSSW_MAJOR_VERSION >= 8
	if (classRequest->GetCollectionType()==ROOT::ESTLType::kSTLvector  &&  classCurrent->GetCollectionType()==ROOT::ESTLType::kSTLvector ){
	  classRequest = classRequest->GetCollectionProxy()->GetValueClass();
	  classCurrent = classCurrent->GetCollectionProxy()->GetValueClass();
	}
	#endif	
	
	if (classCurrent->InheritsFrom(classRequest)) 
		return true;
	std::cerr << "Incompatible types! Requested: " << req << " Found: " << cur << std::endl;
	return false;
}

bool CheckBranch(TBranch *branch, const std::string cname, bool checkDict)
{
	// Check inheritance of requested object
	if (!CheckType(cname, branch->GetClassName()))
		return false;
	// Check members of requested object
	TClass *classBranch = TClass::GetClass(branch->GetClassName());
	std::set<std::string> membersBranch, membersDict, membersDifference;
	GetListOfAllBranches(branch, membersBranch);
	GetListOfAllDataMembers(classBranch, membersDict);
	set_symmetric_difference(
		membersBranch.begin(), membersBranch.end(),
		membersDict.begin(), membersDict.end(),
		inserter(membersDifference, membersDifference.begin()));
	// This check does not yet work with vector
	if (checkDict && (std::string(classBranch->GetName()).find("vector") == std::string::npos) && membersDifference.size())
	{
		std::cerr << "Dictionary for class " << cname << " is not consistent with file content!" << std::endl;
		std::cerr << "\tBranch content: " << membersBranch << std::endl << std::endl;
		std::cerr << "\t  Dict content: " << membersDict << std::endl << std::endl;
		return false;
	}
	return true;
}
