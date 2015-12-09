
#pragma once

#include <map>

#include "Artus/Utility/interface/ArtusLogging.h"

/*
Can be used to retrieve a key from a map and get a solid message which key was not found:

    std::map < int, std::string > myMap;

    myMap [23] = "23";
    myMap [42] = "42";

    std::string const & rr = SafeMap::Get ( myMap, 42 );

*/
class SafeMap
{
public:

	// get value from generic associative containers with default value
	// This function returns, unlike a std::map, not by reference but by value,
	// since returning by reference does not make sense for the default value.
	// (const version)
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value const GetWithDefault(const Container<Key, Value, Args...>& m, Key const& key, const Value & defaultValue)
	{
		typename Container<Key, Value, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
		    return defaultValue;
		return it->second;
	}

	// get value from generic associative containers with default value
	// This function returns, unlike a std::map, not by reference but by value,
	// since returning by reference does not make sense for the default value.
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value GetWithDefault(Container<Key, Value, Args...>& m, Key const& key, Value & defaultValue)
	{
		typename Container<Key, Value, Args...>::iterator it = m.find(key);
		if (it == m.end())
		    return defaultValue;
		return it->second;
	}

	// get value from generic associative containers with default value
	// This function returns, unlike a std::map, not by reference but by value,
	// since returning by reference does not make sense for the default value.
	// (boost::ptr_map version)
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value const GetPtrMapWithDefault(const Container<Key, Value, Args...>& m, Key const& key, const Value & defaultValue)
	{
		typename Container<Key, Value, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
		    return defaultValue;
		return *it->second;
	}

	// get value from generic associative containers without default value
	// fails if key is not available
	// (const version)
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value const& Get(const Container<Key, Value, Args...>& m, Key const& key)
	{
		typename Container<Key, Value, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
		    LOG(FATAL) << "Key \"" << key << "\" can not be found in map.";
		return it->second;
	}

	// get value from generic associative containers without default value
	// fails if key is not available
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value & Get(Container<Key, Value, Args...>& m, Key const& key)
	{
		typename Container<Key, Value, Args...>::iterator it = m.find(key);
		if (it == m.end())
		    LOG(FATAL) << "Key \"" << key << "\" can not be found in map.";
		return it->second;
	}

	// get value from generic associative containers without default value
	// fails if key is not available
	// (boost::ptr_map version)
	template <template<class, class, class...> class Container, typename Key, typename Value, typename... Args>
	static Value const& GetPtrMap(const Container<Key, Value, Args...>& m, Key const& key)
	{
		typename Container<Key, Value, Args...>::const_iterator it = m.find(key);
		if (it == m.end())
		    LOG(FATAL) << "Key \"" << key << "\" can not be found in map.";
		return *it->second;
	}
};	

