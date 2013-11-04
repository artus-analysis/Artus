#pragma once

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "GlobalInclude.h"

/*
 * Enum to store wether the input data if MC or Data
 */
enum InputTypeEnum
{
	McInput, DataInput, GenInput, UnknownInput
};

/*
 * Implements a Setting with automatic read + caching from a Boost PropertyTree
 * You can access the value via myObject.GetSNAME
 */

#define IMPL_SETTING(TYPE, SNAME) \
private: 						  \
TYPE m_##SNAME;					  \
public:  						  \
std::string Key##SNAME () const { return "##SNAME"; }                                         \
std::string FullKey##SNAME () const { return GetSettingsRoot() + "." + #SNAME; }              \
mutable VarCache<TYPE> Cache##SNAME; 														  \
TYPE Get##SNAME ( ) const { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); }  \
       TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME ());     						  \
       Cache##SNAME.SetCache( val ); \
       return val;}

/*
 * Implements a Setting with automatic read + caching from a Boost PropertyTree
 * You can set a default value which will be used if the entry was not found in the PropertyTree
 * You can access the value via myObject.GetSNAME
 */

#define IMPL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) \
private: \
TYPE m_##SNAME;                                                                                                                        \
public: \
std::string Key##SNAME () const { return "##SNAME"; }                                                        \
std::string FullKey##SNAME () const  { return GetSettingsRoot() + "." + #SNAME; }                                                     \
mutable VarCache<TYPE> Cache##SNAME; \
TYPE Get##SNAME ( ) const { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); }         \
       TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME (), DEFAULT_VAL );       \
       Cache##SNAME.SetCache( val ); \
       return val;}            \
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);     \
                                                               Cache##SNAME.SetCache( val );}  \

/*
 * Implements a implicit caching using the VarCache class
 */
#define RETURN_CACHED(CACHE_MEMBER,VALUEPATH) \
{ if (! CACHE_MEMBER.IsCached() ) \
  {             CACHE_MEMBER.SetCache( VALUEPATH );} \
return CACHE_MEMBER.GetValue(); }

/*
 * Convenience class to implement for an arbitrary type TData. This class is useful
 * in conjunction with reading from a Boost PropertyTree or similar
 */
template<class TData>
class VarCache
{
public:
	VarCache() :
		m_isCached(false)
	{

	}

	/*
	 * Sets the cached varible to a certain value.
	 */
	inline void SetCache(TData t) const
	{
		m_val = t;
		m_isCached = true;
	}

	/*
	 * Returns the cached value
	 */
	inline TData GetValue() const
	{
		if (!m_isCached)
			CALIB_LOG_FATAL("not Cached variable used")

			return m_val;
	}

	/*
	 * Returns true, if the value has already been cached
	 */
	inline bool IsCached() const
	{
		return m_isCached;
	}

private:
	mutable bool m_isCached;
	mutable TData m_val;
};

/*
 * This class provides various convenices functions when working with Boost PropertyTrees.
 */
class PropertyTreeSupport
{
public:
static stringvector GetAsStringList(boost::property_tree::ptree * propTree,
		std::string path, bool failIfNotFound = false)
{
	stringvector fvec;
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
				propTree->get_child(path))
		{
			fvec.push_back(v.second.data());
		}
	}
	catch (boost::property_tree::ptree_bad_path& e)
	{
		// no problem, node optional ?
		if (failIfNotFound)
			CALIB_LOG_FATAL("String list on path " + path + " can not be found in configuration file")
	}
	return fvec;
}

static doublevector GetAsDoubleList(boost::property_tree::ptree * propTree,
		std::string path)
{
	doublevector fvec;
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
				propTree->get_child(path))
		{
			fvec.push_back(atof(v.second.data().c_str()));
		}
	}
	catch (boost::property_tree::ptree_bad_path& e)
	{
		// no problem, node optional
	}
	return fvec;
}

static doublevector GetAsDoubleList(boost::property_tree::ptree & propTree,
		std::string path)
{
	doublevector fvec;
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
				propTree.get_child(path))
		{
			fvec.push_back(atof(v.second.data().c_str()));
		}
	}
	catch (boost::property_tree::ptree_bad_path& e)
	{
		// no problem, node optional
	}
	return fvec;
}

static intvector GetAsIntList(boost::property_tree::ptree & propTree,
		std::string path)
{
	intvector fvec;
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
				propTree.get_child(path))
		{
			fvec.push_back(atof(v.second.data().c_str()));
		}
	}
	catch (boost::property_tree::ptree_bad_path& e)
	{
		// no problem, node optional
	}
	return fvec;
}
static intvector GetAsIntList(boost::property_tree::ptree * propTree,
		std::string path)
{
	intvector fvec;
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
				propTree->get_child(path))
		{
			fvec.push_back(atof(v.second.data().c_str()));
		}
	}
	catch (boost::property_tree::ptree_bad_path& e)
	{
		// no problem, node optional
	}
	return fvec;
}
};


