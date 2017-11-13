/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

template<typename T>
std::vector<std::string> FileInterface2::GetLumiNames(bool inherited)
{
	return TreeObjects(lumidata, TypeName<T>::name(), inherited);
}

template<typename T>
std::map<std::string, T*> FileInterface2::GetLumiAll(bool inherited)
{
	std::map<std::string, T*> result;
	std::vector<std::string> names = GetLumiNames<T>(inherited);
	for (std::vector<std::string>::const_iterator it = names.begin(); it < names.end(); ++it)
		result[*it] = GetLumi<T>(*it);
	return result;
}

// Get lumi metadata objects - old style
template<typename T>
T *FileInterface2::GetEvent(run_id run, lumi_id lumi)
{
	GetLumiEntry(run, lumi);
	return GetLumi<T>("lumiInfo");
}

template<typename T>
inline T *FileInterface2::GetEvent(KEventInfo *info_event)
{
	GetLumiEntry(info_event->nRun, info_event->nLumi);
	return GetLumi<T>("lumiInfo");
}

// Get lumi metadata objects
template<typename T>
T *FileInterface2::GetLumi(std::string name, const bool check, const bool def)
{
	if (name == "")
		name = TypeName<T>::name();
	T *result = static_cast<T*>(GetInternal(lumidata, lumiBranches, TypeName<T>::name(), name, check));
	if ((result == nullptr) && def)
		return new T();
	return result;
}

// Get run metadata objects
template<typename T>
T *FileInterface2::GetRun(std::string name, const bool check, const bool def)
{
	if (name == "")
		name = TypeName<T>::name();
	T *result = static_cast<T*>(GetInternal(rundata, runBranches, TypeName<T>::name(), name, check));
	if ((result == nullptr) && def)
		return new T();
	return result;
}
