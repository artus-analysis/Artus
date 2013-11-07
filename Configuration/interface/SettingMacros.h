/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "VarCache.h"

/*
* Implements a Setting with automatic read + caching from a Boost PropertyTree
 * You can access the value via myObject.GetSNAME
 */

#define IMPL_SETTING(TYPE, SNAME) \
private: 						  \
TYPE m_##SNAME;					  \
public:  						  \
std::string Key##SNAME () const { return "##SNAME"; }                                         \
std::string FullKey##SNAME () const { if ( GetPropTreePath() == "" )								\
{return #SNAME;} else {return GetPropTreePath() + "." + #SNAME; }}             \
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
std::string Key##SNAME () const { return "##SNAME"; }                                               \
std::string FullKey##SNAME () const  { if ( GetPropTreePath() == "" )								\
{return #SNAME;} else {return GetPropTreePath() + "." + #SNAME; } }                                 \
mutable VarCache<TYPE> Cache##SNAME; 																\
TYPE Get##SNAME ( ) const { if (Cache##SNAME.IsCached()) { return Cache##SNAME.GetValue(); }         \
       TYPE  val = GetPropTree()->get< TYPE >( FullKey##SNAME (), DEFAULT_VAL );       \
       Cache##SNAME.SetCache( val ); \
       return val;}            \
void Set##SNAME ( TYPE val) { GetPropTree()->put( FullKey##SNAME (), val);     \
                                                               Cache##SNAME.SetCache( val );}  \


