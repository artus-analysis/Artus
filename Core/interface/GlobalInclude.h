/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <exception>

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

//extern std::ofstream * g_logFile;

// value indicating that variable is not defined
#define UNDEFINED_VALUE -999

/// compare floats
#define FLOAT_COMPARE(first, second) (std::abs((first - second) / second) < 0.02)

/// stdout makros
#define LOG(out) { std::cout << out << std::endl; }
#define LOG_FILE(out) { std::cout << out << std::endl;/* (*g_logFile) << out << std::endl;*/ }
#define LOG_FATAL(out) { std::cout << "FATAL: " << out << std::endl;   \
        /*(*g_logFile) << "FATAL:" << out << std::endl;*/                        \
        /*g_logFile->close();*/ std::cout.flush(); assert(false); throw 23; }

// vectors
typedef std::vector<std::string> stringvector;
typedef std::vector<double> doublevector;
typedef std::vector<int> intvector;

/// Implements a property with Get, Set and Has Methods
#define IMPL_PROPERTY(TYPE, SNAME)                          \
private:                                                    \
TYPE m_##SNAME;                                             \
public:                                                     \
TYPE Get##SNAME() const { return m_##SNAME; }               \
bool Has##SNAME() const { return &m_##SNAME; }              \
TYPE const& GetRef##SNAME() const { return m_##SNAME; }     \
TYPE const* GetPt##SNAME() const { return &m_##SNAME; }     \
void Set##SNAME(TYPE val) { m_##SNAME = val; }

#define IMPL_PROPERTY_READONLY(TYPE, SNAME)                 \
private:                                                    \
TYPE m_##SNAME;                                             \
public:                                                     \
TYPE Get##SNAME() const { return m_##SNAME; }               \
private:                                                    \
void Set##SNAME(TYPE val) { m_##SNAME = val; }

/// Tell the compiler if a condition is likely or unlikely
#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)
