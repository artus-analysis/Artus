/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "VarCache.h"
#include "Artus/Utility/interface/Utility.h"


/**
   Implements a Setting with automatic read + caching from a Boost PropertyTree
   You can access the value via myObject.GetSNAME
*/

#define IMPL_SETTING_PRIVATE(TYPE, SNAME, READGLOBAL) \
private: \
	TYPE m_##SNAME; \
public: \
	std::string Key##SNAME () const { \
		return (#SNAME); \
	} \
	std::string FullKey##SNAME () const { \
		if ( READGLOBAL || GetPropTreePath() == "" ) { \
			return #SNAME; \
		} else { \
			return GetPropTreePath() + "." + #SNAME; \
		} \
	} \
	mutable VarCache<TYPE> Cache##SNAME; \
	TYPE Get##SNAME ( ) const { \
		if (Cache##SNAME.IsCached()) { \
			return Cache##SNAME.GetValue(); \
		} \
		TYPE val = TYPE(); \
		try { \
			val = GetPropTree()->get< TYPE >( FullKey##SNAME ()); \
		} catch(...) { \
			try { \
				val = GetPropTree()->get< TYPE >( Key##SNAME ()); \
			} catch(...) { \
				LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			} \
		} \
		Cache##SNAME.SetCache( val ); \
		return val; \
	}

/**
   Implements a Setting with automatic read + caching from a Boost PropertyTree
   You can set a default value which will be used if the entry was not found in the PropertyTree
   You can access the value via myObject.GetSNAME
*/

#define IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, READGLOBAL) \
private: \
	TYPE m_##SNAME; \
public: \
	std::string Key##SNAME () const { \
		return (#SNAME); \
	} \
	std::string FullKey##SNAME () const { \
		if ( READGLOBAL || GetPropTreePath() == "" ) { \
			return #SNAME; \
		} else { \
			return GetPropTreePath() + "." + #SNAME; \
		} \
	} \
	mutable VarCache<TYPE> Cache##SNAME; \
	TYPE Get##SNAME ( ) const { \
		if (Cache##SNAME.IsCached()) { \
			return Cache##SNAME.GetValue(); \
		} \
		TYPE val; \
		try { \
		val = GetPropTree()->get< TYPE >( FullKey##SNAME () ); \
		} catch(...) { \
		val = GetPropTree()->get< TYPE >( Key##SNAME (), DEFAULT_VAL ); \
		} \
		Cache##SNAME.SetCache( val ); \
		return val; \
	}

#define IMPL_SETTING(TYPE, SNAME) IMPL_SETTING_PRIVATE(TYPE, SNAME, false)
#define IMPL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, false)

// #define IMPL_GLOBAL_SETTING(TYPE, SNAME) IMPL_SETTING_PRIVATE(TYPE, SNAME, true)
// #define IMPL_GLOBAL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, true)

//TODO: Sorted lists
#define IMPL_VSETTING_PRIVATE( TYPE, SNAME, READGLOBAL) \
VarCache< std::vector< TYPE > > m_##SNAME; \
virtual std::vector < TYPE > & Get##SNAME () const { \
	if ( !READGLOBAL && PropertyTreeSupport::DoesSettingExist(GetPropTree(), GetPipelinePrefix() + #SNAME)) { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsList< TYPE >(GetPropTree(), GetPipelinePrefix() + #SNAME )); \
	} \
	else if (PropertyTreeSupport::DoesSettingExist(GetPropTree(), #SNAME)) { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsList< TYPE >(GetPropTree(), #SNAME )); \
	} \
	LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
	throw std::runtime_error("Invalid Setting."); \
}
#define IMPL_VSETTING_DEFAULT_PRIVATE( TYPE, SNAME, DEFAULT_VAL, READGLOBAL) \
VarCache< std::vector< TYPE > > m_##SNAME; \
virtual std::vector < TYPE > & Get##SNAME () const { \
	if ( !READGLOBAL && PropertyTreeSupport::DoesSettingExist(GetPropTree(), GetPipelinePrefix() + #SNAME)) { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsList< TYPE >(GetPropTree(), GetPipelinePrefix() + #SNAME )); \
	} \
	else if (PropertyTreeSupport::DoesSettingExist(GetPropTree(), #SNAME)) { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsList< TYPE >(GetPropTree(), #SNAME )); \
	} \
	else { \
		RETURN_CACHED(m_##SNAME, DEFAULT_VAL ); \
	} \
}

#define IMPL_VSETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) IMPL_VSETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, false)
#define IMPL_VSETTING_DEFAULT_GLOBAL(TYPE, SNAME, DEFAULT_VAL) IMPL_VSETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, true)
#define IMPL_VSETTING(TYPE, SNAME) IMPL_VSETTING_PRIVATE(TYPE, SNAME, false)
#define IMPL_VSETTING_GLOBAL(TYPE, SNAME) IMPL_VSETTING_PRIVATE(TYPE, SNAME, true)
