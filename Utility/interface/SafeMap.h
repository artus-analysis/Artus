#pragma once
	
#include <map>
#include <boost/ptr_container/ptr_map.hpp>
	
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

    // const version
    template < class TMap >
    static typename TMap::mapped_type const& Get( TMap const& m, typename TMap::key_type const& k)
    {        
        typename TMap::const_iterator it = m.find(k);
        if (it == m.end())
            LOG(FATAL) << "Item " << k << " can not be found in dictionary.";
        return it->second;
    }

    // non-const version
    template < class TMap >
    static typename TMap::mapped_type & Get( TMap & m, typename TMap::key_type const& k)
    {
        typename TMap::iterator it = m.find(k);
        if (it == m.end())
            LOG(FATAL) << "Item " << k << " can not be found in dictionary.";
        return it->second;
    }

    // for use with boost::ptr_map
    template < class TMap >
    static typename TMap::const_mapped_reference  GetPtrMap(TMap const& m, typename TMap::key_type const& k)
    {
        typename TMap::const_iterator it = m.find(k);
        if (it == m.end())
                LOG(FATAL) << "Item " << k << " can not be found in dictionary.";
        return *it->second;
    }
};	

