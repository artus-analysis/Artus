/* Copyright (c) 2011 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

// Get event metadata objects
template<typename T>
T *FileInterfaceBase::GetEvent()
{
	return GetEvent<T>("eventInfo");
}

// Get event content from files
template<typename T>
T *FileInterfaceBase::GetEvent(const std::string &name, const bool check, const bool def)
{
	T *result = static_cast<T*>(GetInternal(eventdata, eventBranches, TypeName<T>::name(), name, check));
	if ((result == nullptr) && def)
		return new T();
	return result;
}

template<typename T>
std::vector<std::string> FileInterfaceBase::GetEventNames(bool inherited)
{
	return TreeObjects(eventdata, TypeName<T>::name(), inherited);
}

template<typename T>
std::map<std::string, T*> FileInterfaceBase::GetEventAll(bool inherited)
{
	std::map<std::string, T*> result;
	std::vector<std::string> names = GetEventNames<T>(inherited);
	for (std::vector<std::string>::const_iterator it = names.begin(); it < names.end(); ++it)
		result[*it] = GetEvent<T>(*it);
	return result;
}
